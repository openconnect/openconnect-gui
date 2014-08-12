#include "mainwindow.h"
#include "ui_mainwindow.h"
extern "C" {
#include <stdarg.h>
#include <stdio.h>
}
#include <QtConcurrentRun>
#include <QMessageBox>
#include <vpninfo.h>

#define APP_NAME "Qconnect"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    const char *version = openconnect_get_version();
    ui->setupUi(this);
    this->setWindowTitle(QLatin1String("Qconnect (openconnect ")+QLatin1String(version)+QLatin1String(")"));
    this->cmd_fd = -1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateProgressBar(const char *str, int value)
{
    QMutexLocker locker(&this->progress_mutex);
    ui->progressBar->setFormat(QLatin1String(str));
    if (value != -1)
        ui->progressBar->setValue(value);
}

void MainWindow::enableDisconnect(bool val)
{
    ui->disconnectBtn->setEnabled(val);
    ui->connectBtn->setEnabled(val==0?1:0);
}

static void main_loop(VpnInfo *vpninfo, MainWindow *m)
{
    vpninfo->mainloop();
    m->updateProgressBar(vpninfo->last_err, -1);
    m->enableDisconnect(false);
}

void MainWindow::on_disconnectBtn_clicked()
{
    char cmd = OC_CMD_CANCEL;
    if (this->cmd_fd != -1)
        write(this->cmd_fd, &cmd, 1);
}

void MainWindow::on_connectBtn_clicked()
{
    VpnInfo *vpninfo = NULL;
    QFuture<void> result;
    int ret;

    if (ui->lineEdit->text().isEmpty()) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("You need to specify a gateway. E.g. vpn.example.com:443") );
        return;
    }

    vpninfo = new VpnInfo("Qconnect 0.1", this);
    if (vpninfo == NULL)
        return;

    updateProgressBar("%p%", 5);

    vpninfo->parse_url(ui->lineEdit->text().toLocal8Bit().data());

    enableDisconnect(true);

    /* XXX openconnect_set_http_proxy */

    updateProgressBar("%p%", 10);
    ret = vpninfo->connect();
    if (ret != 0) {
        updateProgressBar(vpninfo->last_err, -1);
        goto fail;
    }

    updateProgressBar("%p%", 70);

    ret = vpninfo->dtls_connect();
    if (ret != 0) {
        updateProgressBar(vpninfo->last_err, -1);
        goto fail;
    }

    updateProgressBar("%p%", 100);

    result = QtConcurrent::run (main_loop, vpninfo, this);

    return;
 fail:
    delete vpninfo;
    enableDisconnect(false);
    return;
}
