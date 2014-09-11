/*
 * Copyright (C) 2014 Red Hat
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

#include "editdialog.h"
#include "ui_editdialog.h"
#include <QMessageBox>
#include <QFileDialog>

static int token_tab(int mode)
{
    switch(mode) {
        case OC_TOKEN_MODE_HOTP:
            return 0;
        case OC_TOKEN_MODE_TOTP:
            return 1;
        default:
            return -1;
    }
}

int token_rtab[] = {
    [0] = OC_TOKEN_MODE_HOTP,
    [1] = OC_TOKEN_MODE_TOTP
};

EditDialog::EditDialog(QString server, QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialog)
{
    QString hash, txt;
    ui->setupUi(this);
    int type;

    this->ss = new StoredServer(settings);
    this->ss->load(server);
    this->ss->set_window(this);

    txt = ss->get_label();
    ui->labelEdit->setText(txt);
    if (txt.isEmpty() == true) {
        ui->labelEdit->setText(server);
    }
    ui->groupnameEdit->setText(ss->get_groupname());
    ui->usernameEdit->setText(ss->get_username());
    ui->gatewayEdit->setText(ss->get_servername());
    ui->userCertHash->setText(ss->get_client_cert_hash());
    ui->caCertHash->setText(ss->get_ca_cert_hash());
    ui->batchModeBox->setChecked(ss->get_batch_mode());
    ui->minimizeBox->setChecked(ss->get_minimize());
    ui->proxyBox->setChecked(ss->get_proxy());

    type = ss->get_token_type();
    if (type >= 0) {
        ui->tokenBox->setCurrentIndex(token_tab(ss->get_token_type()));
        ui->tokenEdit->setText(ss->get_token_str());
    }

    ss->get_server_hash(hash);
    ui->serverCertHash->setText(hash);
}

EditDialog::~EditDialog()
{
    delete ui;
    delete ss;
}

void EditDialog::on_buttonBox_accepted()
{
    int type;
    if (ui->gatewayEdit->text().isEmpty() == true) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("You need to specify a gateway. E.g. vpn.example.com:443") );
        return;
    }

    if (ui->labelEdit->text().isEmpty() == true) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("You need to specify a name for this connection. E.g. 'My company'") );
        return;
    }

    if (ss->client_is_complete() != true) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("There is a client certificate specified but no key!") );
        return;
    }
    ss->set_label(ui->labelEdit->text());
    ss->set_username(ui->usernameEdit->text());
    ss->set_servername(ui->gatewayEdit->text());
    ss->set_batch_mode(ui->batchModeBox->isChecked());
    ss->set_minimize(ui->minimizeBox->isChecked());
    ss->set_proxy(ui->proxyBox->isChecked());

    type = ui->tokenBox->currentIndex();
    if (type != -1 && ui->tokenEdit->text().isEmpty()==false) {
        ss->set_token_str(ui->tokenEdit->text());
        ss->set_token_type(token_rtab[type]);
    } else {
        ss->set_token_str("");
        ss->set_token_type(-1);
    }

    ss->save();
    this->close();
}

void EditDialog::on_buttonBox_rejected()
{
    this->close();
}

void EditDialog::on_userCertButton_clicked()
{
    QString filename;

    filename = QFileDialog::getOpenFileName(this,
        tr("Open certificate"), "", tr("Certificate Files (*.crt *.pem *.der *.p12)"));

    if (filename.isEmpty() == false) {
        if (ss->set_client_cert(filename) != 0) {
            QMessageBox mbox;
            mbox.setText(tr("Cannot import certificate."));
            if (ss->last_err.isEmpty() == false)
                mbox.setInformativeText(ss->last_err);
            mbox.exec();
        } else {
            ui->userCertEdit->setText(filename);
            ui->userCertHash->setText(ss->get_client_cert_hash());
        }
    }
}

void EditDialog::on_userKeyButton_clicked()
{
    QString filename;

    filename = QFileDialog::getOpenFileName(this,
        tr("Open private key"), "", tr("Private key Files (*.key *.pem *.der *.p8 *.p12)"));

    if (filename.isEmpty() == false) {
        if (ss->set_client_key(filename) != 0) {
            QMessageBox mbox;
            mbox.setText(tr("Cannot import certificate."));
            if (ss->last_err.isEmpty() == false)
                mbox.setInformativeText(ss->last_err);
            mbox.exec();
        } else {
            ui->userKeyEdit->setText(filename);
        }
    }
}

void EditDialog::on_caCertButton_clicked()
{
    QString filename;

    filename = QFileDialog::getOpenFileName(this,
        tr("Open certificate"), "", tr("Certificate Files (*.crt *.pem *.der)"));

    if (filename.isEmpty() == false) {
        if (ss->set_ca_cert(filename) != 0) {
            QMessageBox mbox;
            mbox.setText(tr("Cannot import certificate."));
            if (ss->last_err.isEmpty() == false)
                mbox.setInformativeText(ss->last_err);
            mbox.exec();
        } else {
            ui->caCertEdit->setText(filename);
            ui->caCertHash->setText(ss->get_client_cert_hash());
        }
    }
}

void EditDialog::on_userCertClear_clicked()
{
    ss->clear_cert();
    ui->userCertEdit->clear();
    ui->userCertHash->clear();
}

void EditDialog::on_userKeyClear_clicked()
{
    ss->clear_key();
    ui->userKeyEdit->clear();
}

void EditDialog::on_caCertClear_clicked()
{
    ss->clear_ca();
    ui->caCertEdit->clear();
    ui->caCertHash->clear();
}

void EditDialog::on_serverCertClear_clicked()
{
    ss->clear_server_hash();
}

void EditDialog::on_tokenClear_clicked()
{
    ui->tokenBox->setCurrentIndex(-1);
    ui->tokenEdit->clear();
}

void EditDialog::on_toolButton_clicked()
{
    ss->clear_groupname();
    ui->groupnameEdit->clear();
}
