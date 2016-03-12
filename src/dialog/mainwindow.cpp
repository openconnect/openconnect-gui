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
#include "config.h"
#include "editdialog.h"
#include "logdialog.h"
#include "storage.h"
#include "ui_mainwindow.h"
#include "vpninfo.h"

extern "C" {
#include <gnutls/gnutls.h>
}
#include <QCloseEvent>
#include <QDateTime>
#include <QDialog>
#include <QFutureWatcher>
#include <QLineEdit>
#include <QMessageBox>
#include <QUrl>
#include <QtConcurrent/QtConcurrentRun>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkProxyFactory>
#include <QtNetwork/QNetworkProxyQuery>

#include <cstdarg>
#include <cstdio>

#ifdef _WIN32
#define pipe_write(x, y, z) send(x, y, z, 0)
#else
#define pipe_write(x, y, z) write(x, y, z)
#endif

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    blink_timer = new QTimer(this);
    this->cmd_fd = INVALID_SOCKET;

    connect(ui->actionQuit, &QAction::triggered,
            qApp, &QApplication::quit);

    connect(blink_timer, &QTimer::timeout,
            this, &MainWindow::blink_ui,
            Qt::QueuedConnection);
    connect(timer, &QTimer::timeout,
            this, &MainWindow::request_update_stats,
            Qt::QueuedConnection);
    connect(ui->comboBox->lineEdit(), &QLineEdit::returnPressed,
            this, &MainWindow::on_connectClicked,
            Qt::QueuedConnection);
    connect(this, &MainWindow::vpn_status_changed_sig,
            this, &MainWindow::changeStatus,
            Qt::QueuedConnection);
    connect(ui->connectionButton, &QPushButton::clicked,
            this, &MainWindow::on_connectClicked,
            Qt::QueuedConnection);

    connect(this, &MainWindow::log_changed,
            this, &MainWindow::writeProgressBar,
            Qt::QueuedConnection);
    connect(this, &MainWindow::stats_changed_sig,
            this, &MainWindow::statsChanged,
            Qt::QueuedConnection);

    ui->iconLabel->setPixmap(OFF_ICON);
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        createActions();
        createTrayIcon();

        connect(m_trayIcon, &QSystemTrayIcon::activated,
                this, &MainWindow::iconActivated);

        QIcon icon;
        icon.addPixmap(TRAY_OFF_ICON, QIcon::Normal, QIcon::Off);
        m_trayIcon->setIcon(icon);
        m_trayIcon->show();
    }
    else {
        updateProgressBar(QLatin1String("System doesn't support tray icon"), false);
        m_trayIcon = nullptr;
    }

    readSettings();
}

static void term_thread(MainWindow* m, SOCKET* fd)
{
    char cmd = OC_CMD_CANCEL;

    if (*fd != INVALID_SOCKET) {
        int ret = pipe_write(*fd, &cmd, 1);
        if (ret < 0)
            m->updateProgressBar(QObject::tr("term_thread: IPC error: ") + QString::number(net_errno));
        *fd = INVALID_SOCKET;
        ms_sleep(200);
    }
    else {
        m->vpn_status_changed(STATUS_DISCONNECTED);
    }
}

MainWindow::~MainWindow()
{
    int counter = 10;
    if (this->timer->isActive()) {
        timer->stop();
    }

    if (this->futureWatcher.isRunning() == true) {
        term_thread(this, &this->cmd_fd);
    }
    while (this->futureWatcher.isRunning() == true && counter > 0) {
        ms_sleep(200);
        counter--;
    }

    delete ui;
    delete timer;
    delete blink_timer;

    writeSettings();
}

void MainWindow::disable_cmd_fd()
{
    cmd_fd = INVALID_SOCKET;
}

void MainWindow::vpn_status_changed(int connected)
{
    emit vpn_status_changed_sig(connected);
}

void MainWindow::vpn_status_changed(int connected, QString& dns, QString& ip, QString& ip6, QString& cstp_cipher, QString& dtls_cipher)
{
    this->dns = dns;
    this->ip = ip;
    this->ip6 = ip6;
    this->dtls_cipher = dtls_cipher;
    this->cstp_cipher = cstp_cipher;

    emit vpn_status_changed_sig(connected);
}

QStringList* MainWindow::get_log()
{
    return &this->log;
}

QString value_to_string(uint64_t bytes)
{
    if (bytes > 1000 && bytes < 1000 * 1000) {
        bytes /= 1000;
        QString r = QString::number((int)bytes);
        r += QObject::tr(" KB");
        return r;
    }
    else if (bytes >= 1000 * 1000 && bytes < 1000 * 1000 * 1000) {
        bytes /= 1000 * 1000;
        QString r = QString::number((int)bytes);
        r += QObject::tr(" MB");
        return r;
    }
    else if (bytes >= 1000 * 1000 * 1000) {
        bytes /= 1000 * 1000 * 1000;
        QString r = QString::number((int)bytes);
        r += QObject::tr(" GB");
        return r;
    }
    else {
        QString r = QString::number((int)bytes);
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

void MainWindow::updateStats(const struct oc_stats* stats, QString dtls)
{
    emit stats_changed_sig(value_to_string(stats->tx_bytes),
                           value_to_string(stats->rx_bytes),
                           dtls);
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

void MainWindow::set_settings(QSettings* s)
{
    this->settings = s; // LCA: drop settings
    reload_settings();
}

void MainWindow::writeProgressBar(const QString& str)
{
    ui->statusBar->showMessage(str, 20 * 1000);
}

void MainWindow::updateProgressBar(const QString& str)
{
    updateProgressBar(str, true);
}

void MainWindow::updateProgressBar(QString str, bool show) // LCA: const ???
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
    }
    else {
        ui->iconLabel->setPixmap(CONNECTING_ICON2);
    }
    ++t;
}

void MainWindow::changeStatus(int val)
{
    if (val == STATUS_CONNECTED) {

        blink_timer->stop();

        ui->comboBox->setEnabled(false);
        ui->toolButton->setEnabled(false);
        ui->toolButton_2->setEnabled(false);

        ui->iconLabel->setPixmap(ON_ICON);
        ui->connectionButton->setIcon(QIcon(":/new/resource/images/process-stop.png"));
        ui->connectionButton->setText(tr("Disconnect"));

        QIcon icon(TRAY_ON_ICON);
        m_trayIcon->setIcon(icon);

        this->ui->IPLabel->setText(ip);
        this->ui->IP6Label->setText(ip6);
        this->ui->DNSLabel->setText(dns);
        this->ui->CSTPLabel->setText(cstp_cipher);
        this->ui->DTLSLabel->setText(dtls_cipher);

        timer->start(UPDATE_TIMER);

        if (this->minimize_on_connect) {
            if (m_trayIcon) {
                this->hideWindow();
                m_trayIcon->showMessage(QLatin1String("Connected"), QLatin1String("You were connected to ") + ui->comboBox->currentText(),
                                        QSystemTrayIcon::Information,
                                        10000);
            }
            else {
                this->setWindowState(Qt::WindowMinimized);
            }
        }
        disconnect(ui->connectionButton, &QPushButton::clicked,
                   this, &MainWindow::on_connectClicked);
        connect(ui->connectionButton, &QPushButton::clicked,
                this, &MainWindow::on_disconnectClicked,
                Qt::QueuedConnection);
    }
    else if (val == STATUS_CONNECTING) {

        if (m_trayIcon) {
            QIcon icon(TRAY_OFF_ICON);
            m_trayIcon->setIcon(icon);
        }

        ui->comboBox->setEnabled(false);
        ui->toolButton->setEnabled(false);
        ui->toolButton_2->setEnabled(false);

        ui->iconLabel->setPixmap(CONNECTING_ICON);
        ui->connectionButton->setIcon(QIcon(":/new/resource/images/process-stop.png"));
        ui->connectionButton->setText(tr("Cancel"));
        blink_timer->start(1500);
    }
    else if (val == STATUS_DISCONNECTED) {
        blink_timer->stop();
        if (this->timer->isActive()) {
            timer->stop();
        }
        disable_cmd_fd();

        ui->CSTPLabel->clear();
        ui->DTLSLabel->clear();
        ui->IPLabel->clear();
        ui->DNSLabel->clear();
        ui->IP6Label->clear();
        this->updateProgressBar(QObject::tr("Disconnected"));

        ui->comboBox->setEnabled(true);
        ui->toolButton->setEnabled(true);
        ui->toolButton_2->setEnabled(true);

        ui->iconLabel->setPixmap(OFF_ICON);
        ui->connectionButton->setIcon(QIcon(":/new/resource/images/network-wired.png"));
        ui->connectionButton->setText(tr("Connect"));

        if (m_trayIcon) {
            QIcon icon(TRAY_OFF_ICON);
            m_trayIcon->setIcon(icon);

            if (this->isHidden() == true)
                m_trayIcon->showMessage(QLatin1String("Disconnected"), QLatin1String("You were disconnected from the VPN"),
                                        QSystemTrayIcon::Warning,
                                        10000);
        }
        disconnect(ui->connectionButton, &QPushButton::clicked,
                   this, &MainWindow::on_disconnectClicked);
        connect(ui->connectionButton, &QPushButton::clicked,
                this, &MainWindow::on_connectClicked,
                Qt::QueuedConnection);
    }
}

static void main_loop(VpnInfo* vpninfo, MainWindow* m)
{
    m->vpn_status_changed(STATUS_CONNECTING);

    bool pass_was_empty;
    pass_was_empty = vpninfo->ss->get_password().isEmpty();

    QString ip, ip6, dns, cstp, dtls;

    int ret = 0;
    bool retry = false;
    int retries = 2;
    do {
        retry = false;
        ret = vpninfo->connect();
        if (ret != 0) {
            if (retries-- <= 0)
                goto fail;

            QString oldpass, oldgroup;
            bool reset_password = false;
            if (pass_was_empty != true) {
                /* authentication failed in batch mode? switch to non
                 * batch and retry */
                oldpass = vpninfo->ss->get_password();
                oldgroup = vpninfo->ss->get_groupname();
                vpninfo->ss->clear_password();
                vpninfo->ss->clear_groupname();
                retry = true;
                reset_password = true;
                m->updateProgressBar(QObject::tr("Authentication failed in batch mode, retrying with batch mode disabled"));
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

fail: // LCA: drop this 'goto' and optimize values...
    m->vpn_status_changed(STATUS_DISCONNECTED);

    delete vpninfo;
}

void MainWindow::on_disconnectClicked()
{
    if (this->timer->isActive()) {
        this->timer->stop();
    }
    this->updateProgressBar(QObject::tr("Disconnecting..."));
    term_thread(this, &this->cmd_fd);
}

void MainWindow::on_connectClicked()
{
    VpnInfo* vpninfo = nullptr;
    StoredServer* ss = new StoredServer(this->settings); // LCA: drop settings
    QFuture<void> future;
    QString name, str, url;
    QList<QNetworkProxy> proxies;
    QUrl turl;
    QNetworkProxyQuery query;

    if (this->cmd_fd != INVALID_SOCKET) {
        QMessageBox::information(this,
                                 qApp->applicationName(),
                                 tr("A previous VPN instance is still running (socket is active)"));
        return;
    }

    if (this->futureWatcher.isRunning() == true) {
        QMessageBox::information(this,
                                 qApp->applicationName(),
                                 tr("A previous VPN instance is still running"));
        return;
    }

    if (ui->comboBox->currentText().isEmpty()) {
        QMessageBox::information(this,
                                 qApp->applicationName(),
                                 tr("You need to specify a gateway. e.g. vpn.example.com:443"));
        return;
    }

    name = ui->comboBox->currentText();
    ss->load(name);
    turl.setUrl("https://" + ss->get_servername());
    query.setUrl(turl);

    /* ss is now deallocated by vpninfo */
    vpninfo = new VpnInfo(QString("%1 %2").arg(qApp->applicationName()).arg(qApp->applicationVersion()), ss, this);
    if (vpninfo == nullptr) {
        QMessageBox::information(this,
                                 qApp->applicationName(),
                                 tr("There was an issue initializing the VPN."));
        goto fail;
    }

    this->minimize_on_connect = vpninfo->get_minimize();

    vpninfo->parse_url(ss->get_servername().toLocal8Bit().data());

    this->cmd_fd = vpninfo->get_cmd_fd();
    if (this->cmd_fd == INVALID_SOCKET) {
        QMessageBox::information(this,
                                 qApp->applicationName(),
                                 tr("There was an issue establishing IPC with openconnect; try restarting the application."));
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

            str = proxies.at(0).user() + ":" + proxies.at(0).password() + "@" + proxies.at(0).hostName();
            if (proxies.at(0).port() != 0) {
                str += ":" + QString::number(proxies.at(0).port());
            }
            this->updateProgressBar(tr("Setting proxy to: ") + str);
            openconnect_set_http_proxy(vpninfo->vpninfo, str.toLatin1().data());
        }
    }

    future = QtConcurrent::run(main_loop, vpninfo, this);

    this->futureWatcher.setFuture(future);

    return;
fail: // LCA: remote 'fail' label :/
    if (vpninfo != nullptr)
        delete vpninfo;
    return;
}

void MainWindow::on_toolButton_clicked()
{
    EditDialog dialog(ui->comboBox->currentText(), this->settings); // LCA: drop settings
    dialog.exec();

    int idx = ui->comboBox->currentIndex();
    reload_settings();
    if (idx < ui->comboBox->maxVisibleItems() && idx >= 0) {
        ui->comboBox->setCurrentIndex(idx);
    }
    else if (ui->comboBox->maxVisibleItems() == 0) {
        ui->comboBox->setCurrentIndex(0);
    }
    // LCA: else ???
}

void MainWindow::on_toolButton_2_clicked()
{
    if (ui->comboBox->currentText().isEmpty() == false) {
        QMessageBox mbox;
        mbox.setText(tr("Are you sure you want to remove '%1' host?").arg(ui->comboBox->currentText()));
        mbox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        mbox.setDefaultButton(QMessageBox::Cancel);
        mbox.setButtonText(QMessageBox::Ok, tr("Remove"));
        if (mbox.exec() == QMessageBox::Ok) {
            remove_server(settings, ui->comboBox->currentText());
            reload_settings(); // LCA: remove this feature...
        }
    }
}

static LogDialog* logdialog = nullptr;
void MainWindow::clear_logdialog()
{
    logdialog = nullptr;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (logdialog) {
        logdialog->close();
        logdialog = nullptr;
    }

    if (m_trayIcon && m_trayIcon->isVisible()) {
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
    if (logdialog == nullptr) {
        logdialog = new LogDialog(this->log);

        QObject::connect(this, &MainWindow::log_changed,
                         logdialog, &LogDialog::append,
                         Qt::QueuedConnection);
        QObject::connect(logdialog, &LogDialog::clear_log,
                         this, &MainWindow::clear_log,
                         Qt::QueuedConnection);
        QObject::connect(logdialog, &LogDialog::clear_logdialog,
                         this, &MainWindow::clear_logdialog,
                         Qt::DirectConnection);

        logdialog->show();
        logdialog->raise();
        logdialog->activateWindow();
    }
    else {
        logdialog->raise();
    }
}

void MainWindow::on_about()
{
    QString txt = QLatin1String("<h2>") + QLatin1String(appDescriptionLong) + QLatin1String("</h2>") + tr("Version: ") + QLatin1String("<i>") + QLatin1String(appVersion) + QLatin1String("</i><br><br>");
    txt += tr("Based on:<br>- libopenconnect: ") + QLatin1String(openconnect_get_version()) + tr("<br>- GnuTLS: ") + QLatin1String(gnutls_check_version(nullptr));
    txt += tr("<br><br>Copyright 2014 Red Hat Inc.");
    txt += tr("<br>Openconnect-gui comes with ABSOLUTELY NO WARRANTY. This is free software, "
              "and you are welcome to redistribute it under the conditions "
              "of the GNU General Public License version 2.");

    QMessageBox::about(this, "", txt);
}

void MainWindow::on_aboutQt()
{
    qApp->aboutQt();
}

void MainWindow::request_update_stats()
{
    char cmd = OC_CMD_STATS;
    if (this->cmd_fd != INVALID_SOCKET) {
        int ret = pipe_write(this->cmd_fd, &cmd, 1);
        if (ret < 0) {
            this->updateProgressBar(QObject::tr("update_stats: IPC error: ") + QString::number(net_errno));
            if (this->timer->isActive())
                this->timer->stop();
        }
    }
    else {
        this->updateProgressBar(QObject::tr("update_stats: invalid socket"));
        if (this->timer->isActive())
            this->timer->stop();
    }
}

void MainWindow::toggleWindow()
{
    if (m_trayIcon == nullptr) {
        return;
    }

    if (this->isHidden()) {
        setVisible(true);
    }
    else {
        setVisible(false);
    }
}

void MainWindow::hideWindow()
{
    if (m_trayIcon == nullptr) {
        return;
    }

    if (this->isHidden() == false) {
        setVisible(false);
    }
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    resize(settings.value("size").toSize());
    if (settings.contains("pos")) {
        move(settings.value("pos").toPoint());
    }
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
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
    m_trayIconMenu = new QMenu(this);
    m_trayIconMenu->addAction(m_minimizeAction);
    m_trayIconMenu->addAction(m_restoreAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayIconMenu);
}

void MainWindow::setVisible(bool visible)
{
    m_minimizeAction->setEnabled(visible);
    m_restoreAction->setEnabled(isMaximized() || !visible);
    QMainWindow::setVisible(visible);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        this->toggleWindow();
        break;
    default:;
    }
}

void MainWindow::createActions()
{
    m_minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(m_minimizeAction, &QAction::triggered,
            this, &MainWindow::hide);

    m_restoreAction = new QAction(tr("&Restore"), this);
    connect(m_restoreAction, &QAction::triggered,
            this, &MainWindow::showNormal);

    m_quitAction = new QAction(tr("&Quit"), this);
    connect(m_quitAction, &QAction::triggered,
            qApp, &QApplication::quit);
}