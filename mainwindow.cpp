/*
 * Copyright (C) 2014 Red Hat
 *
 * This file is part of openconnect-gui.
 *
 * openconnect-gui is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
#include <QLineEdit>
#include <QFutureWatcher>

#include "logdialog.h"
#include "editdialog.h"

#ifdef _WIN32
# define pipe_write(x,y,z) send(x,y,z,0)
#else
# define pipe_write(x,y,z) write(x,y,z)
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    const char *version = openconnect_get_version();
    ui->setupUi(this);
    this->setWindowTitle(QLatin1String("openconnect "VERSION" (lib ")+QLatin1String(version)+QLatin1String(")"));

    timer = new QTimer(this);
    this->cmd_fd = INVALID_SOCKET;

    connect(timer, SIGNAL(timeout()), this, SLOT(request_update_stats()), Qt::QueuedConnection);
    connect(ui->comboBox->lineEdit(), SIGNAL(returnPressed()), this, SLOT(on_connectBtn_clicked()), Qt::QueuedConnection);
    connect(this, SIGNAL(vpn_status_changed_sig(int)), this, SLOT(changeStatus(int)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(log_changed(QString)), this, SLOT(writeProgressBar(QString)), Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(stats_changed_sig(QString, QString)), this, SLOT(statsChanged(QString, QString)), Qt::QueuedConnection);
    ui->iconLabel->setPixmap(OFF_ICON);
}

static void term_thread(MainWindow *m, SOCKET *fd)
{
    char cmd = OC_CMD_CANCEL;

    if (*fd != INVALID_SOCKET) {
        int ret = pipe_write(*fd, &cmd, 1);
        if (ret < 0)
          m->updateProgressBar(QObject::tr("term_thread: IPC error: ")+QString::number(net_errno));
        *fd = INVALID_SOCKET;
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
    while(this->futureWatcher.isRunning() == true && counter > 0) {
        ms_sleep(200);
        counter--;
    }
    delete ui;
    delete timer;
}

QString
value_to_string(uint64_t bytes)
{
    QString r;
    if (bytes > 1000 && bytes < 1000 * 1000) {
        bytes /= 1000;
        r = QString::number((int)bytes);
        r += QObject::tr(" KB");
        return r;
    } else if (bytes >= 1000 * 1000 && bytes < 1000 * 1000 * 1000) {
        bytes /= 1000*1000;
        r = QString::number((int)bytes);
        r += QObject::tr(" MB");
        return r;
    } else if (bytes >= 1000 * 1000 * 1000) {
        bytes /= 1000*1000*1000;
        r = QString::number((int)bytes);
        r += QObject::tr(" GB");
        return r;
    } else {
        r = QString::number((int)bytes);
        r += QObject::tr(" bytes");
        return r;
    }
}

void MainWindow::statsChanged(QString tx, QString rx)
{
    ui->lcdDown->setText(tx);
    ui->lcdUp->setText(rx);
}

void MainWindow::updateStats(const struct oc_stats *stats)
{
    emit stats_changed_sig(value_to_string(stats->tx_bytes), value_to_string(stats->rx_bytes));
}

void MainWindow::reload_settings()
{
    QStringList servers;
    ui->comboBox->clear();

    servers = get_server_list(this->settings);

    for (int i = 0;i<servers.size();i++) {
        ui->comboBox->addItem(servers.at(i));
    }
}

void MainWindow::set_settings(QSettings *s)
{
    this->settings = s;
    reload_settings();
};

void MainWindow::writeProgressBar(QString str)
{
    ui->statusBar->showMessage(str, 20*1000);
}

void MainWindow::updateProgressBar(QString str)
{
    QMutexLocker locker(&this->progress_mutex);
    if (str.isEmpty() == false) {
        QDateTime now;
        emit log_changed(str);
        str.prepend(now.currentDateTime().toString("yyyy-MM-dd hh:mm "));
        log.append(str);
    }
}

void MainWindow::changeStatus(int val)
{
    if (val == STATUS_CONNECTED) {
        ui->iconLabel->setPixmap(ON_ICON);
        ui->disconnectBtn->setEnabled(true);
        ui->connectBtn->setEnabled(false);
        
	this->ui->IPLabel->setText(ip);
    	this->ui->IP6Label->setText(ip6);
    	this->ui->DNSLabel->setText(dns);

        timer->start(UPDATE_TIMER);
    } else if (val == STATUS_CONNECTING) {
        ui->iconLabel->setPixmap(CONNECTING_ICON);
        ui->disconnectBtn->setEnabled(true);
        ui->connectBtn->setEnabled(false);
    } else if (val == STATUS_DISCONNECTED){
        if (this->timer->isActive())
            timer->stop();
        disable_cmd_fd();

        ui->IPLabel->setText("");
        ui->DNSLabel->setText("");
        ui->IP6Label->setText("");

        ui->disconnectBtn->setEnabled(false);
        ui->connectBtn->setEnabled(true);
        ui->iconLabel->setPixmap(OFF_ICON);
    }
}

static void main_loop(VpnInfo *vpninfo, MainWindow *m)
{
    int ret;
    QString ip, ip6, dns;

    m->vpn_status_changed(STATUS_CONNECTING);

    ret = vpninfo->connect();
    if (ret != 0) {
        m->updateProgressBar(vpninfo->last_err);
        goto fail;
    }

    ret = vpninfo->dtls_connect();
    if (ret != 0) {
        m->updateProgressBar(vpninfo->last_err);
        goto fail;
    }


    vpninfo->get_info(dns, ip, ip6);
    m->vpn_status_changed(STATUS_CONNECTED, dns, ip, ip6);

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
    QFuture<void> future;
    QString name;

    if (ui->connectBtn->isEnabled() == false) {
        return;
    }

    if (this->cmd_fd != INVALID_SOCKET) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("A previous VPN instance is still running (socket is active)") );
        return;
    }

    if (this->futureWatcher.isRunning() == true) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("A previous VPN instance is still running") );
        return;
    }

    if (ui->comboBox->currentText().isEmpty()) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("You need to specify a gateway. E.g. vpn.example.com:443") );
        return;
    }

    name = ui->comboBox->currentText();
    if (ss->load(name) == 0) {
        ss->set_servername(name);
    }

    /* ss is now deallocated by vpninfo */
    vpninfo = new VpnInfo(tr(APP_STRING), ss, this);
    if (vpninfo == NULL) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("There was an issue initializing the VPN.") );
        goto fail;
    }

    vpninfo->parse_url(ss->get_servername().toLocal8Bit().data());

    this->cmd_fd = vpninfo->get_cmd_fd();
    if (this->cmd_fd == INVALID_SOCKET) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("There was an issue establishing IPC with openconnect; try restarting the application.") );
        goto fail;
    }

    /* XXX openconnect_set_http_proxy */

    future = QtConcurrent::run (main_loop, vpninfo, this);

    this->futureWatcher.setFuture(future);

    return;
 fail:
    if (vpninfo != NULL)
        delete vpninfo;
    return;
}


void MainWindow::on_toolButton_clicked()
{
    EditDialog dialog(ui->comboBox->currentText(), this->settings);
    dialog.exec();
    reload_settings();
}

void MainWindow::on_toolButton_2_clicked()
{
    if (ui->comboBox->currentText().isEmpty() == false) {
        QMessageBox mbox;
        int ret;

        mbox.setText(QObject::tr("Are you sure you want to remove ")+ui->comboBox->currentText()+"?");
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

void MainWindow::on_toolButton_3_clicked()
{
    LogDialog dialog(this->log);

    QObject::connect(this, SIGNAL(log_changed(QString)), &dialog, SLOT(append(QString)), Qt::QueuedConnection);

    dialog.exec();
}

void MainWindow::request_update_stats()
{
    char cmd = OC_CMD_STATS;
    if (this->cmd_fd != INVALID_SOCKET) {
        int ret = pipe_write(this->cmd_fd, &cmd, 1);
        if (ret < 0) {
            this->updateProgressBar(QObject::tr("update_stats: IPC error: ")+QString::number(net_errno));
            if (this->timer->isActive())
                this->timer->stop();
        }
    } else {
    	this->updateProgressBar(QObject::tr("update_stats: invalid socket"));
        if (this->timer->isActive())
            this->timer->stop();
    }
}

