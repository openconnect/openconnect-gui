/*
 * Copyright (C) 2014 Red Hat
 *
 * This file is part of qconnect.
 *
 * Qconnect is free software: you can redistribute it and/or modify
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    const char *version = openconnect_get_version();
    ui->setupUi(this);
    this->setWindowTitle(QLatin1String("Qconnect (openconnect ")+QLatin1String(version)+QLatin1String(")"));

    timer = new QTimer(this);
    this->cmd_fd = -1;

    connect(timer, SIGNAL(timeout()), this, SLOT(request_update_stats()));
    connect(ui->comboBox->lineEdit(), SIGNAL(returnPressed()), this, SLOT(on_connectBtn_clicked()), Qt::QueuedConnection);
    connect(this, SIGNAL(vpn_status_changed_sig(bool)), this, SLOT(enableDisconnect(bool)));

}

static void term_thread(int *fd)
{
    char cmd = OC_CMD_CANCEL;
    if (*fd != -1) {
        write(*fd, &cmd, 1);
        close(*fd);
        *fd = -1;
    }
}

MainWindow::~MainWindow()
{
    term_thread(&this->cmd_fd);
    timer->stop();
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
        r += " KB";
        return r;
    } else if (bytes >= 1000 * 1000 && bytes < 1000 * 1000 * 1000) {
        bytes /= 1000*1000;
        r = QString::number((int)bytes);
        r += " MB";
        return r;
    } else if (bytes >= 1000 * 1000 * 1000) {
        bytes /= 1000*1000*1000;
        r = QString::number((int)bytes);
        r += " GB";
        return r;
    } else {
        r = QString::number((int)bytes);
        r += " bytes";
        return r;
    }
}
void MainWindow::updateStats(const struct oc_stats *stats)
{
    ui->lcdDown->setText(value_to_string(stats->tx_bytes));
    ui->lcdUp->setText(value_to_string(stats->rx_bytes));
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

void MainWindow::updateProgressBar(QString str)
{
    QMutexLocker locker(&this->progress_mutex);
    ui->statusBar->showMessage(str, 20*1000);
    if (str.isEmpty() == false) {
        QDateTime now;
        str.prepend(now.currentDateTime().toString("yyyy-MM-dd hh:mm "));
        log.append(str);
        emit log_changed(str);
    }
}

void MainWindow::enableDisconnect(bool val)
{
    if (val == true) {
        ui->disconnectBtn->setEnabled(true);
        ui->connectBtn->setEnabled(false);
    } else {
        timer->stop();
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
    vpninfo->mainloop();
    m->updateProgressBar(vpninfo->last_err);

    m->vpn_status_changed(false);
    m->disable_cmd_fd();
    m->stop_timer();

    delete vpninfo;
    vpninfo = NULL;
}

void MainWindow::on_disconnectBtn_clicked()
{
    term_thread(&this->cmd_fd);
}

void MainWindow::on_connectBtn_clicked()
{
    VpnInfo *vpninfo = NULL;
    StoredServer *ss = new StoredServer(this->settings);
    QFuture<void> result;
    QString name;
    int ret;
    QString ip, ip6, dns;

    if (this->cmd_fd != -1)
        return;

    if (ui->comboBox->currentText().isEmpty()) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("You need to specify a gateway. E.g. vpn.example.com:443") );
        return;
    }

    timer->start(UPDATE_TIMER);

    name = ui->comboBox->currentText();
    if (ss->load(name) == 0) {
        ss->set_servername(name);
    }

    vpninfo = new VpnInfo(APP_STRING, ss, this);
    if (vpninfo == NULL)
        return;

    vpninfo->parse_url(ss->get_servername().toLocal8Bit().data());

    enableDisconnect(true);

    /* XXX openconnect_set_http_proxy */

    ret = vpninfo->connect();
    if (ret != 0) {
        updateProgressBar(vpninfo->last_err);
        goto fail;
    }

    ret = vpninfo->dtls_connect();
    if (ret != 0) {
        updateProgressBar(vpninfo->last_err);
        goto fail;
    }

    updateProgressBar("saving peer's information");
    vpninfo->ss->save();

    ui->iconLabel->setPixmap(ON_ICON);

    vpninfo->get_info(dns, ip, ip6);;
    ui->IPLabel->setText(ip);
    ui->IP6Label->setText(ip6);

    ui->DNSLabel->setText(dns);

    result = QtConcurrent::run (main_loop, vpninfo, this);
    this->cmd_fd = vpninfo->get_cmd_fd();

    return;
 fail:
    delete vpninfo;
    vpninfo = NULL;
    enableDisconnect(false);
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

        mbox.setText(QLatin1String("Are you sure you want to remove ")+ui->comboBox->currentText()+"?");
        mbox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        mbox.setDefaultButton(QMessageBox::Cancel);
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
    QFutureWatcher<void> futureWatcher;

    QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(cancel()));
    QObject::connect(this, SIGNAL(log_changed(QString)), &dialog, SLOT(append(QString)));

    dialog.exec();

    futureWatcher.waitForFinished();
}

void MainWindow::request_update_stats()
{
    char cmd = OC_CMD_STATS;
    if (this->cmd_fd != -1)
        write(this->cmd_fd, &cmd, 1);
}

void MainWindow::stop_timer()
{
    timer->stop();
}
