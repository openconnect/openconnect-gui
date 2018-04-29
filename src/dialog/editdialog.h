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

#pragma once

#include <QDialog>

class StoredServer;
class QSettings;

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog {
    Q_OBJECT
public:
    EditDialog(QString server, QWidget* parent = 0);
    ~EditDialog();

    QString getEditedProfileName() const;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_userCertButton_clicked();
    void on_userKeyButton_clicked();
    void on_caCertButton_clicked();
    void on_userCertClear_clicked();
    void on_userKeyClear_clicked();
    void on_caCertClear_clicked();
    void on_serverCertClear_clicked();
    void on_tokenClear_clicked();
    void on_groupnameClear_clicked();
    void on_loadWinCert_clicked();

    void on_groupnameEdit_textChanged(const QString& arg1);
    void on_caCertEdit_textChanged(const QString& arg1);
    void on_serverCertHash_textChanged(const QString& arg1);
    void on_tokenEdit_textChanged(const QString& arg1);
    void on_userCertEdit_textChanged(const QString& arg1);
    void on_userKeyEdit_textChanged(const QString& arg1);

    void on_loadWinCertList_itemSelectionChanged();

private:
    void load_win_certs();

    struct win_cert_st {
        QString label;
        QString key_url;
        QString cert_url;
    };

    Ui::EditDialog* ui;
    std::vector<win_cert_st> winCerts;
    StoredServer* ss;
};
