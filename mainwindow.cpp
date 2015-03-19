/*
 * Copyright (C) 2014, 2015 Red Hat
 *
 * This file is part of openconnect-gui.
 *
 * openconnect-gui is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
extern "C" {
#include <stdarg.h>
#include <stdio.h>
}
#include <QtConcurrent/QtConcurrentRun>
#include <QDateTime>
#include <QMessageBox>
#include <vpninfo.h>
#include <storage.h>
#include <gnutls/gnutls.h>
#include <QLineEdit>
#include <QCloseEvent>
#include <QDialog>
#include <QFutureWatcher>
#include <QtNetwork/QNetworkProxyFactory>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkProxyQuery>
#include <QUrl>
#include "logdialog.h"
#include "editdialog.h"
#ifdef _WIN32
#define pipe_write(x,y,z) send(x,y,z,0)
#else
#define pipe_write(x,y,z) write(x,y,z)
#endif
MainWindow::MainWindow(QWidget * parent):
QMainWindow(parent), ui(new Ui::MainWindow)
{
    const char *version = openconnect_get_version();
    QString txt;

    ui->setupUi(this);
    this->setWindowTitle(QLatin1String("openconnect " VERSION));

    txt = QLatin1String("Based on libopenconnect ") + QLatin1String(version);
    txt +=
        QLatin1String("\nGnuTLS: ") + QLatin1String(gnutls_check_version(NULL));
    ui->versionLabel->setText(txt);

    timer = new QTimer(this);
    blink_timer = new QTimer(this);
    this->cmd_fd = INVALID_SOCKET;

    connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(blink_timer, SIGNAL(timeout(void)), this, SLOT(blink_ui(void)),
            Qt::QueuedConnection);
    connect(timer, SIGNAL(timeout()), this, SLOT(request_update_stats()),
            Qt::QueuedConnection);
    connect(ui->comboBox->lineEdit(), SIGNAL(returnPressed()), this,
            SLOT(on_connectBtn_clicked()), Qt::QueuedConnection);
    connect(this, SIGNAL(vpn_status_changed_sig(int)), this,
            SLOT(changeStatus(int)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(log_changed(QString)), this,
                     SLOT(writeProgressBar(QString)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(stats_changed_sig(QString, QString, QString)),
                     this, SLOT(statsChanged(QString, QString, QString)),
                     Qt::QueuedConnection);
    ui->iconLabel->setPixmap(OFF_ICON);
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        QIcon icon;
        createActions();
        createTrayIcon();

        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

        icon.addPixmap(TRAY_OFF_ICON, QIcon::Normal, QIcon::Off);
        trayIcon->setIcon(icon);
        trayIcon->show();
    } else {
        updateProgressBar(QLatin1String("System doesn't support tray icon"),
                          false);
        trayIcon = NULL;
    }
}

static void term_thread(MainWindow * m, SOCKET * fd)
{
    char cmd = OC_CMD_DETACH;

    if (*fd != INVALID_SOCKET) {
        int ret = pipe_write(*fd, &cmd, 1);
        if (ret < 0)
            m->updateProgressBar(QObject::tr("term_thread: IPC error: ") +
                                 QString::number(net_errno));
        *fd = INVALID_SOCKET;
        ms_sleep(200);
    } else {
        m->vpn_status_changed(STATUS_DISCONNECTED);
    }
}

MainWindow::~MainWindow()
{
    int counter = 10;
    if (this->timer->isActive())
        timer->stop();

    if (this->futureWatcher.isRunning() == true) {
        term_thread(this, &this->cmd_fd);
    }
    while (this->futureWatcher.isRunning() == true && counter > 0) {
        ms_sleep(200);
        counter--;
    }
    delete ui;
    delete timer;
}

QString value_to_string(uint64_t bytes)
{
    QString r;
    if (bytes > 1000 && bytes < 1000 * 1000) {
        bytes /= 1000;
        r = QString::number((int)bytes);
        r += QObject::tr(" KB");
        return r;
    } else if (bytes >= 1000 * 1000 && bytes < 1000 * 1000 * 1000) {
        bytes /= 1000 * 1000;
        r = QString::number((int)bytes);
        r += QObject::tr(" MB");
        return r;
    } else if (bytes >= 1000 * 1000 * 1000) {
        bytes /= 1000 * 1000 * 1000;
        r = QString::number((int)bytes);
        r += QObject::tr(" GB");
        return r;
    } else {
        r = QString::number((int)bytes);
        r += QObject::tr(" bytes");
        return r;
    }
}

void MainWindow::statsChanged(QString tx, QString rx, QString dtls)
{
    ui->lcdDown->setText(rx);
    ui->lcdUp->setText(tx);
    ui->DTLSLabel->setText(dtls);
}

void MainWindow::updateStats(const struct oc_stats *stats, QString dtls)
{
    emit stats_changed_sig(value_to_string(stats->tx_bytes),
                           value_to_string(stats->rx_bytes), dtls);
}

void MainWindow::reload_settings()
{
    QStringList servers;
    ui->comboBox->clear();

    servers = get_server_list(this->settings);

    for (int i = 0; i < servers.size(); i++) {
        ui->comboBox->addItem(servers.at(i));
    }
}

void MainWindow::set_settings(QSettings * s)
{
    this->settings = s;
    reload_settings();
};

void MainWindow::writeProgressBar(QString str)
{
    ui->statusBar->showMessage(str, 20 * 1000);
}

void MainWindow::updateProgressBar(QString str)
{
    updateProgressBar(str, true);
}

void MainWindow::updateProgressBar(QString str, bool show)
{
    QMutexLocker locker(&this->progress_mutex);
    if (str.isEmpty() == false) {
        QDateTime now;
        if (show == true)
            emit log_changed(str);
        str.prepend(now.currentDateTime().toString("yyyy-MM-dd hh:mm "));
        log.append(str);
    }
}

void MainWindow::clear_log()
{
    this->log.clear();
}

void MainWindow::blink_ui()
{
    static unsigned t = 1;

    if (t % 2 == 0) {
        ui->iconLabel->setPixmap(CONNECTING_ICON);
    } else {
        ui->iconLabel->setPixmap(CONNECTING_ICON2);
    }
    t++;
}

void MainWindow::changeStatus(int val)
{
    QIcon icon;
    if (val == STATUS_CONNECTED) {

        blink_timer->stop();
        ui->iconLabel->setPixmap(ON_ICON);
        ui->disconnectBtn->setEnabled(true);
        ui->connectBtn->setEnabled(false);

        icon.addPixmap(TRAY_ON_ICON, QIcon::Normal, QIcon::Off);
        trayIcon->setIcon(icon);

        this->ui->IPLabel->setText(ip);
        this->ui->IP6Label->setText(ip6);
        this->ui->DNSLabel->setText(dns);
        this->ui->CSTPLabel->setText(cstp_cipher);
        this->ui->DTLSLabel->setText(dtls_cipher);

        timer->start(UPDATE_TIMER);

        if (this->minimize_on_connect) {
            if (trayIcon) {
                this->hideWindow();
                trayIcon->showMessage(QLatin1String("Connected"), QLatin1String("You were connected to ")+ui->comboBox->currentText(),
                                      QSystemTrayIcon::Information,
                                      10000);
            } else {
                this->setWindowState(Qt::WindowMinimized);
            }
        }

    } else if (val == STATUS_CONNECTING) {

        if (trayIcon) {
            icon.addPixmap(TRAY_OFF_ICON, QIcon::Normal, QIcon::Off);
            trayIcon->setIcon(icon);
        }
        ui->iconLabel->setPixmap(CONNECTING_ICON);
        ui->disconnectBtn->setEnabled(true);
        ui->connectBtn->setEnabled(false);
        blink_timer->start(1500);
    } else if (val == STATUS_DISCONNECTED) {
        blink_timer->stop();
        if (this->timer->isActive())
            timer->stop();
        disable_cmd_fd();

        ui->CSTPLabel->setText("");
        ui->DTLSLabel->setText("");
        ui->IPLabel->setText("");
        ui->DNSLabel->setText("");
        ui->IP6Label->setText("");
        this->updateProgressBar(QObject::tr("Disconnected"));

        ui->disconnectBtn->setEnabled(false);
        ui->connectBtn->setEnabled(true);
        ui->iconLabel->setPixmap(OFF_ICON);

        if (trayIcon) {
            icon.addPixmap(TRAY_OFF_ICON, QIcon::Normal, QIcon::Off);
            trayIcon->setIcon(icon);

            if (this->isHidden() == true)
                trayIcon->showMessage(QLatin1String("Disconnected"), QLatin1String("You were disconnected from the VPN"),
                                      QSystemTrayIcon::Warning,
                                      10000);
        }
    }
}

static void main_loop(VpnInfo * vpninfo, MainWindow * m)
{
    int ret;
    QString ip, ip6, dns, cstp, dtls;
    bool retry = false;
    QString oldpass, oldgroup;
    bool reset_password = false;
    int retries = 2;
    bool pass_was_empty;

    m->vpn_status_changed(STATUS_CONNECTING);

    pass_was_empty = vpninfo->ss->get_password().isEmpty();

    do {
        retry = false;
        ret = vpninfo->connect();
        if (ret != 0) {
	    if (retries-- <= 0)
               goto fail;

            if (pass_was_empty != true) {
                /* authentication failed in batch mode? switch to non
                 * batch and retry */
                oldpass = vpninfo->ss->get_password();
                oldgroup = vpninfo->ss->get_groupname();
                vpninfo->ss->clear_password();
                vpninfo->ss->clear_groupname();
                retry = true;
                reset_password = true;
                m->updateProgressBar(QObject::tr
                                     ("Authentication failed in batch mode, retrying with batch mode disabled"));
                vpninfo->reset_vpn();
                continue;
            }

            /* if we didn't manage to connect on a retry, the failure reason
             * may not have been a changed password, reset it */
            if (reset_password == true) {
                vpninfo->ss->set_password(oldpass);
                vpninfo->ss->set_groupname(oldgroup);
            }

            m->updateProgressBar(vpninfo->last_err);
            goto fail;
        }


    } while (retry == true);

    ret = vpninfo->dtls_connect();
    if (ret != 0) {
        m->updateProgressBar(vpninfo->last_err);
    }

    vpninfo->get_info(dns, ip, ip6);
    vpninfo->get_cipher_info(cstp, dtls);
    m->vpn_status_changed(STATUS_CONNECTED, dns, ip, ip6, cstp, dtls);

    vpninfo->ss->save();

    vpninfo->mainloop();

 fail:
    m->vpn_status_changed(STATUS_DISCONNECTED);

    delete vpninfo;
    return;
}

void MainWindow::on_disconnectBtn_clicked()
{
    if (this->timer->isActive())
        this->timer->stop();
    this->updateProgressBar(QObject::tr("Disconnecting..."));
    term_thread(this, &this->cmd_fd);
}

void MainWindow::on_connectBtn_clicked()
{
    VpnInfo *vpninfo = NULL;
    StoredServer *ss = new StoredServer(this->settings);
    QFuture < void >future;
    QString name, str, url;
    QList < QNetworkProxy > proxies;
    QUrl turl;
    QNetworkProxyQuery query;

    if (ui->connectBtn->isEnabled() == false) {
        return;
    }

    if (this->cmd_fd != INVALID_SOCKET) {
        QMessageBox::information(this,
                                 tr(APP_NAME),
                                 tr
                                 ("A previous VPN instance is still running (socket is active)"));
        return;
    }

    if (this->futureWatcher.isRunning() == true) {
        QMessageBox::information(this,
                                 tr(APP_NAME),
                                 tr
                                 ("A previous VPN instance is still running"));
        return;
    }

    if (ui->comboBox->currentText().isEmpty()) {
        QMessageBox::information(this,
                                 tr(APP_NAME),
                                 tr
                                 ("You need to specify a gateway. E.g. vpn.example.com:443"));
        return;
    }

    name = ui->comboBox->currentText();
    ss->load(name);
    turl.setUrl("https://" + ss->get_servername());
    query.setUrl(turl);

    /* ss is now deallocated by vpninfo */
    vpninfo = new VpnInfo(tr(APP_STRING), ss, this);
    if (vpninfo == NULL) {
        QMessageBox::information(this,
                                 tr(APP_NAME),
                                 tr
                                 ("There was an issue initializing the VPN."));
        goto fail;
    }

    this->minimize_on_connect = vpninfo->get_minimize();

    vpninfo->parse_url(ss->get_servername().toLocal8Bit().data());

    this->cmd_fd = vpninfo->get_cmd_fd();
    if (this->cmd_fd == INVALID_SOCKET) {
        QMessageBox::information(this,
                                 tr(APP_NAME),
                                 tr
                                 ("There was an issue establishing IPC with openconnect; try restarting the application."));
        goto fail;
    }

    proxies = QNetworkProxyFactory::systemProxyForQuery(query);
    if (proxies.size() > 0 && proxies.at(0).type() != QNetworkProxy::NoProxy) {
        if (proxies.at(0).type() == QNetworkProxy::Socks5Proxy)
            url = "socks5://";
        else if (proxies.at(0).type() == QNetworkProxy::HttpCachingProxy
                 || proxies.at(0).type() == QNetworkProxy::HttpProxy)
            url = "http://";

        if (url.isEmpty() == false) {

            str =
                proxies.at(0).user() + ":" + proxies.at(0).password() + "@" +
                proxies.at(0).hostName();
            if (proxies.at(0).port() != 0) {
                str += ":" + QString::number(proxies.at(0).port());
            }
            this->updateProgressBar(tr("Setting proxy to: ") + str);
            openconnect_set_http_proxy(vpninfo->vpninfo, str.toAscii().data());
        }
    }

    future = QtConcurrent::run(main_loop, vpninfo, this);

    this->futureWatcher.setFuture(future);

    return;
 fail:
    if (vpninfo != NULL)
        delete vpninfo;
    return;
}

void MainWindow::on_toolButton_clicked()
{
    int idx;
    EditDialog dialog(ui->comboBox->currentText(), this->settings);
    dialog.exec();
    idx = ui->comboBox->currentIndex();
    reload_settings();
    if (idx < ui->comboBox->maxVisibleItems() && idx >= 0) {
        ui->comboBox->setCurrentIndex(idx);
    } else if (ui->comboBox->maxVisibleItems() == 0) {
        ui->comboBox->setCurrentIndex(0);
    }
}

void MainWindow::on_toolButton_2_clicked()
{
    if (ui->comboBox->currentText().isEmpty() == false) {
        QMessageBox mbox;
        int ret;

        mbox.setText(QObject::tr("Are you sure you want to remove ") +
                     ui->comboBox->currentText() + "?");
        mbox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        mbox.setDefaultButton(QMessageBox::Cancel);
        mbox.setButtonText(QMessageBox::Ok, tr("Remove"));

        ret = mbox.exec();
        if (ret == QMessageBox::Ok) {
            remove_server(settings, ui->comboBox->currentText());
            reload_settings();
        }
    }
}

static LogDialog *logdialog = NULL;
void MainWindow::clear_logdialog()
{
    logdialog = NULL;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    if (logdialog) {
        logdialog->close();
        logdialog = NULL;
    }

    if (trayIcon && trayIcon->isVisible()) {
    	static int shown = 0;

    	if (shown == 0) {
	        QMessageBox::information(this, tr("Systray"),
        	                         tr("The program will keep running in the "
                	                    "system tray. To terminate the program, "
                        	            "choose <b>Quit</b> in the system tray entry."));
		shown = 1;
	}
        hideWindow();
        event->ignore();
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if (logdialog == NULL) {
        logdialog = new LogDialog(this->log);

        QObject::connect(this, SIGNAL(log_changed(QString)), logdialog,
                         SLOT(append(QString)), Qt::QueuedConnection);
        QObject::connect(logdialog, SIGNAL(clear_log(void)), this,
                         SLOT(clear_log(void)), Qt::QueuedConnection);
        QObject::connect(logdialog, SIGNAL(clear_logdialog(void)), this,
                         SLOT(clear_logdialog(void)), Qt::DirectConnection);

        logdialog->show();
        logdialog->raise();
        logdialog->activateWindow();
    } else {
        logdialog->raise();
    }
}

void MainWindow::request_update_stats()
{
    char cmd = OC_CMD_STATS;
    if (this->cmd_fd != INVALID_SOCKET) {
        int ret = pipe_write(this->cmd_fd, &cmd, 1);
        if (ret < 0) {
            this->updateProgressBar(QObject::tr("update_stats: IPC error: ") +
                                    QString::number(net_errno));
            if (this->timer->isActive())
                this->timer->stop();
        }
    } else {
        this->updateProgressBar(QObject::tr("update_stats: invalid socket"));
        if (this->timer->isActive())
            this->timer->stop();
    }
}

void MainWindow::toggleWindow()
{
    if (trayIcon == NULL)
        return;

    if (this->isHidden()) {
        setVisible(true);
    } else {
        setVisible(false);
    }
}

void MainWindow::hideWindow()
{
    if (trayIcon == NULL)
        return;

    if (this->isHidden() == false) {
        setVisible(false);
    }
}


/****************************************************************************
 **
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 **     the names of its contributors may be used to endorse or promote
 **     products derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/
void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::setVisible(bool visible)
{
    minimizeAction->setEnabled(visible);
    restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        this->toggleWindow();
        break;
    default:
        ;
    }
}

void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}


