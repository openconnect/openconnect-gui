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
#include <QSettings>

LogDialog::LogDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LogDialog)
{
    ui->setupUi(this);

    loadSettings();
    ui->listWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);

    for (const auto& msg : Logger::instance().getMessages()) {
        ui->listWidget->addItem(msg.text);
    }

    if (ui->checkBox_autoScroll->checkState() == Qt::Checked) {
        ui->listWidget->scrollToBottom();
    }

    connect(&Logger::instance(), &Logger::newLogMessage,
            this, &LogDialog::append, Qt::QueuedConnection);
}

LogDialog::~LogDialog()
{
    disconnect(&Logger::instance(), &Logger::newLogMessage,
               this, &LogDialog::append);

    delete ui;
}

void LogDialog::reject()
{
    emit clear_logdialog();
    QDialog::reject();
}

void LogDialog::on_pushButtonSelectAll_clicked()
{
    ui->listWidget->selectAll();
}

void LogDialog::append(const Logger::Message& message)
{
    ui->listWidget->addItem(message.text);
    if (ui->checkBox_autoScroll->checkState() == Qt::Checked) {
        ui->listWidget->scrollToBottom();
    }
}

void LogDialog::closeEvent(QCloseEvent* event)
{
    saveSettings();

    QDialog::closeEvent(event);
}

void LogDialog::on_pushButtonClear_clicked()
{
    if (ui->listWidget->count()) {
        if (QMessageBox::question(this,
                "",
                tr("Are you sure you want to clear the log?"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No)
            == QMessageBox::Yes) {
            emit clear_log();
            ui->listWidget->clear();
        }
    }
}

void LogDialog::on_pushButtonCopy_clicked()
{
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
    QString text;
    foreach (QListWidgetItem* item, items) {
        text.append(QString("%1%2").arg(item->text()).arg('\n'));
    }

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void LogDialog::onItemSelectionChanged()
{
    ui->pushButtonCopy->setEnabled(!ui->listWidget->selectedItems().empty());
}

void LogDialog::loadSettings()
{
    QSettings settings;
    settings.beginGroup("LogWindow");
    if (settings.contains("size")) {
        resize(settings.value("size").toSize());
    }
    if (settings.contains("pos")) {
        move(settings.value("pos").toPoint());
    }
    settings.endGroup();
}

void LogDialog::saveSettings()
{

    QSettings settings;
    settings.beginGroup("LogWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}
