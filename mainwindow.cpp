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
//#include <QtConcurrentRun>
#include <QMessageBox>
#include <vpninfo.h>
#include <storage.h>
#include <QLineEdit>
#include "editdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    const char *version = openconnect_get_version();
    ui->setupUi(this);
    this->setWindowTitle(QLatin1String("Qconnect (openconnect ")+QLatin1String(version)+QLatin1String(")"));

    connect(ui->comboBox->lineEdit(), SIGNAL(returnPressed()), this, SLOT(on_connectBtn_clicked()), Qt::QueuedConnection);

}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::updateProgressBar(const char *str)
{
    QMutexLocker locker(&this->progress_mutex);
    ui->statusBar->showMessage(QLatin1String(str));
    std::cerr << str << "\n";
}

void MainWindow::enableDisconnect(bool val)
{
    ui->disconnectBtn->setEnabled(val);
    ui->connectBtn->setEnabled(val==0?1:0);
}

static void main_loop(VpnInfo *vpninfo, MainWindow *m)
{
    vpninfo->mainloop();
    m->updateProgressBar(vpninfo->last_err);
    m->enableDisconnect(false);

    delete vpninfo;
}

void MainWindow::on_disconnectBtn_clicked()
{
    this->vpninfo->disconnect();
}

void MainWindow::on_connectBtn_clicked()
{
    VpnInfo *vpninfo = NULL;
    StoredServer *ss = new StoredServer(this->settings);
    QFuture<void> result;
    QString name;
    int ret;

    updateProgressBar("");

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

    result = QtConcurrent::run (main_loop, vpninfo, this);
    this->vpninfo = vpninfo;

    return;
 fail:
    delete vpninfo;
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
        remove_server(settings, ui->comboBox->currentText());
        reload_settings();
    }
}
