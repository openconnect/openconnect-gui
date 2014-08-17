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

#ifndef EDITDIALOG_H
#define EDITDIALOG_H

#include <storage.h>
#include <QDialog>
#include "common.h"

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditDialog(QString server, QSettings *settings, QWidget *parent = 0);
    ~EditDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_userCertButton_clicked();

    void on_userKeyButton_clicked();

    void on_caCertButton_clicked();

    void on_userCertClear_clicked();

    void on_userKeyClear_clicked();

    void on_caCertClear_clicked();

private:
    Ui::EditDialog *ui;
    StoredServer *ss;
};

#endif // EDITDIALOG_H
