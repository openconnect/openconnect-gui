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

#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog {
    Q_OBJECT
public : LogDialog(QStringList items,
                   QWidget* parent = 0);
    ~LogDialog();

private slots:
    void append(QString item);
    void reject();
    void cancel();

    void on_pushButtonClear_clicked();
    void on_pushButtonClose_clicked();
    void on_pushButtonSelectAll_clicked();
    void on_pushButtonCopy_clicked();

    void onItemSelectionChanged();

signals:
    void clear_log(void);
    void clear_logdialog(void);

private:
    Ui::LogDialog* ui;
    QStringList text;
};

#endif // LOGDIALOG_H
