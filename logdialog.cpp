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


#include "logdialog.h"
#include "ui_logdialog.h"
#include <QClipboard>

LogDialog::LogDialog(QStringList items, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
    this->text = items;
    ui->listWidget->addItems(text);
    ui->listWidget->scrollToBottom();
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::on_pushButton_clicked()
{
    this->close();
}

void LogDialog::on_copyButton_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();

    clipboard->setText(text.join("\n"));
}

void LogDialog::append(QString item)
{
    this->text.append(item);
    ui->listWidget->addItem(item);
}
