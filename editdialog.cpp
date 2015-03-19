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

#include "common.h"
#include "editdialog.h"
#include "ui_editdialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QListWidget>
#include <QItemSelectionModel>

#ifdef USE_SYSTEM_KEYS
#include <gnutls/system-keys.h>
#endif

static int token_tab(int mode)
{
    switch (mode) {
    case OC_TOKEN_MODE_HOTP:
        return 0;
    case OC_TOKEN_MODE_TOTP:
        return 1;
    case OC_TOKEN_MODE_STOKEN:
        return 2;
    default:
        return -1;
    }
}

int token_rtab[] = {
    [0] = OC_TOKEN_MODE_HOTP,
    [1] = OC_TOKEN_MODE_TOTP,
    [2] = OC_TOKEN_MODE_STOKEN
};

void EditDialog::load_win_certs()
{
#ifdef USE_SYSTEM_KEYS
    gnutls_system_key_iter_t iter = NULL;
    char *label;
    char *cert_url;
    char *key_url;
    int ret, idx = -1;
    int row = 0;
    QString prekey = ss->get_key_url();

    if (prekey.isEmpty() == false) {
        ui->userKeyEdit->setText(prekey);
    }

    this->winCerts.clear();
    ui->loadWinCertList->clear();

    do {
        ret =
            gnutls_system_key_iter_get_info(&iter, &cert_url, &key_url, &label,
                                            NULL, 0);
        if (ret >= 0) {
            win_cert_st st;
            QString l;
            if (label != NULL)
                l = QString::fromUtf8(label);
            else
                l = QString::fromUtf8(cert_url);
            ui->loadWinCertList->addItem(l);
            if (prekey.isEmpty() == false) {
                if (QString::compare(prekey, QString::fromUtf8(key_url),
                                     Qt::CaseSensitive) == 0) {
                    idx = row;
                }
            }
            row++;

            st.label = l;
            st.key_url = QString::fromUtf8(key_url);
            st.cert_url = QString::fromUtf8(cert_url);
            this->winCerts.push_back(st);
        }
    } while (ret >= 0);

    if (idx != -1) {
        ui->loadWinCertList->setCurrentRow(idx);
        ui->loadWinCertList->item(idx)->setSelected(true);
    }
    gnutls_system_key_iter_deinit(iter);
#endif
}

 EditDialog::EditDialog(QString server, QSettings * settings, QWidget * parent):
QDialog(parent),
ui(new Ui::EditDialog)
{
    QString hash, txt;
    ui->setupUi(this);
    int type, ret;

    this->ss = new StoredServer(settings);

    ret = this->ss->load(server);
    if (ret < 0) {
        QMessageBox::information(this,
                                 tr(APP_NAME),
                                 ss->last_err.isEmpty()?
                                 tr("Some server information failed to load") :
                                 ss->last_err);
    }

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
    ui->disableUDP->setChecked(ss->get_disable_udp());

    // Load the windows certificates
    load_win_certs();

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
    QMessageBox mbox;
    int type;

    if (ui->gatewayEdit->text().isEmpty() == true) {
        QMessageBox::information(this,
                                 tr(APP_NAME),
                                 tr
                                 ("You need to specify a gateway. E.g. vpn.example.com:443"));
        return;
    }

    if (ui->labelEdit->text().isEmpty() == true) {
        QMessageBox::information(this,
                                 tr(APP_NAME),
                                 tr
                                 ("You need to specify a name for this connection. E.g. 'My company'"));
        return;
    }

    if (ui->caCertEdit->text().isEmpty() == false) {
        if (ss->set_ca_cert(ui->caCertEdit->text()) != 0) {
            mbox.setText(tr("Cannot import CA certificate."));
            if (ss->last_err.isEmpty() == false)
                mbox.setInformativeText(ss->last_err);
            mbox.exec();
            return;
        } else {
            ui->caCertHash->setText(ss->get_ca_cert_hash());
        }
    }

    if (ui->userKeyEdit->text().isEmpty() == false) {
        if (ss->set_client_key(ui->userKeyEdit->text()) != 0) {
            mbox.setText(tr("Cannot import user key."));
            if (ss->last_err.isEmpty() == false)
                mbox.setInformativeText(ss->last_err);
            mbox.exec();
            return;
        }
    }

    if (ui->userCertEdit->text().isEmpty() == false) {
        if (ss->set_client_cert(ui->userCertEdit->text()) != 0) {

            mbox.setText(tr("Cannot import user certificate."));
            if (ss->last_err.isEmpty() == false)
                mbox.setInformativeText(ss->last_err);
            mbox.exec();
            return;
        } else {
            ui->userCertHash->setText(ss->get_client_cert_hash());
        }
    }

    if (ss->client_is_complete() != true) {
        QMessageBox::information(this,
                                 tr(APP_NAME),
                                 tr
                                 ("There is a client certificate specified but no key!"));
        return;
    }
    ss->set_label(ui->labelEdit->text());
    ss->set_username(ui->usernameEdit->text());
    ss->set_servername(ui->gatewayEdit->text());
    ss->set_batch_mode(ui->batchModeBox->isChecked());
    ss->set_minimize(ui->minimizeBox->isChecked());
    ss->set_proxy(ui->proxyBox->isChecked());
    ss->set_disable_udp(ui->disableUDP->isChecked());

    type = ui->tokenBox->currentIndex();
    if (type != -1 && ui->tokenEdit->text().isEmpty() == false) {
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
                                            tr("Open certificate"), "",
                                            tr
                                            ("Certificate Files (*.crt *.pem *.der *.p12)"));

    ui->userCertEdit->setText(filename);
}

void EditDialog::on_userKeyButton_clicked()
{
    QString filename;

    filename = QFileDialog::getOpenFileName(this,
                                            tr("Open private key"), "",
                                            tr
                                            ("Private key Files (*.key *.pem *.der *.p8 *.p12)"));

    ui->userKeyEdit->setText(filename);
}

void EditDialog::on_caCertButton_clicked()
{
    QString filename;

    filename = QFileDialog::getOpenFileName(this,
                                            tr("Open certificate"), "",
                                            tr
                                            ("Certificate Files (*.crt *.pem *.der)"));

    ui->caCertEdit->setText(filename);
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

void EditDialog::on_loadWinCert_clicked()
{
    int idx = ui->loadWinCertList->currentRow();
    win_cert_st st;
    if (idx < 0 || this->winCerts.size() <= (unsigned)idx)
        return;

    st = this->winCerts.at(idx);
    ui->userCertEdit->setText(st.cert_url);
    ui->userKeyEdit->setText(st.key_url);
}
