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

#include "editdialog.h"
#include "ui_editdialog.h"
#include <QMessageBox>
#include <QFileDialog>

EditDialog::EditDialog(QString server, QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialog)
{
    QString hash;
    ui->setupUi(this);

    this->ss = new StoredServer(settings);
    this->ss->load(server);
    this->ss->set_window(this);

    ui->groupnameEdit->setText(ss->get_groupname());
    ui->usernameEdit->setText(ss->get_username());
    ui->gatewayEdit->setText(ss->get_servername());
    ui->userCertHash->setText(ss->get_client_cert_hash());
    ui->caCertHash->setText(ss->get_ca_cert_hash());
    ui->batchModeBox->setChecked(ss->get_batch_mode());

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
    if (ui->gatewayEdit->text().isEmpty() == true) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("You need to specify a gateway. E.g. vpn.example.com:443") );
        return;
    }

    if (ss->client_is_complete() != true) {
        QMessageBox::information(
            this,
            tr(APP_NAME),
            tr("There is a client certificate specified but no key!") );
        return;
    }
    ss->set_username(ui->usernameEdit->text());
    ss->set_servername(ui->gatewayEdit->text());
    ss->set_batch_mode(ui->batchModeBox->isChecked());

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
            mbox.setText("Cannot import certificate.");
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
            mbox.setText("Cannot import certificate.");
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
            mbox.setText("Cannot import certificate.");
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
}

void EditDialog::on_userKeyClear_clicked()
{
    ss->clear_key();
}

void EditDialog::on_caCertClear_clicked()
{
    ss->clear_ca();
}

void EditDialog::on_serverCertClear_clicked()
{
    ss->clear_server_hash();
}
