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

EditDialog::EditDialog(QString server, QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialog)
{
    ui->setupUi(this);

    this->ss = new StoredServer(settings);
    this->ss->load(server);

    ui->groupnameEdit->setText(ss->get_groupname());
    ui->usernameEdit->setText(ss->get_username());
    ui->gatewayEdit->setText(ss->get_servername());
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
    ss->set_username(ui->usernameEdit->text());
    ss->set_servername(ui->gatewayEdit->text());

    ss->save();
    this->close();
}

void EditDialog::on_buttonBox_rejected()
{
    this->close();
}
