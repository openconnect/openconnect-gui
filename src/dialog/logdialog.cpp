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

#include "logdialog.h"
#include "ui_logdialog.h"
#include <QClipboard>
#include <QMessageBox>

LogDialog::LogDialog(QStringList items, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LogDialog)
{
    ui->setupUi(this);

    ui->listWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);

    ui->listWidget->addItems(items);
    ui->listWidget->scrollToBottom();
    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &LogDialog::onItemSelectionChanged);
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::reject()
{
    emit clear_logdialog();
    QDialog::reject();
}

void LogDialog::cancel()
{
    emit clear_logdialog();
    this->close();
}

void LogDialog::on_pushButtonClose_clicked()
{
    this->cancel();
}

void LogDialog::on_pushButtonSelectAll_clicked()
{
    ui->listWidget->selectAll();
}

void LogDialog::append(QString item)
{
    ui->listWidget->addItem(item);
    if (ui->checkBox_autoScroll->checkState() == Qt::Checked) {
        ui->listWidget->scrollToBottom();
    }
}

void LogDialog::on_pushButtonClear_clicked()
{
    if (ui->listWidget->count()) {
        QMessageBox mbox;
        int ret;

        mbox.setText(QObject::tr("Are you sure you want to clear the log?"));
        mbox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        mbox.setDefaultButton(QMessageBox::Cancel);
        mbox.setButtonText(QMessageBox::Ok, tr("Clear"));

        ret = mbox.exec();
        if (ret == QMessageBox::Ok) {
            emit clear_log();
            ui->listWidget->clear();
        }
    }
}

void LogDialog::on_pushButtonCopy_clicked()
{
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
    QString text;
    foreach(QListWidgetItem* item, items) {
        text.append(QString("%1%2").arg(item->text()).arg('\n'));
    }

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void LogDialog::onItemSelectionChanged()
{
    ui->pushButtonCopy->setEnabled(!ui->listWidget->selectedItems().empty());
}
