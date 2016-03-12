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

struct win_cert_st {
    QString label;
    QString key_url;
    QString cert_url;
};

namespace Ui {
class EditDialog;
}

class EditDialog : public QDialog {
    Q_OBJECT public : EditDialog(QString server,
                                 QSettings* settings,
                                 QWidget* parent = 0);
    ~EditDialog();

private slots:
    void load_win_certs();
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
    void on_toolButton_clicked();
    void on_loadWinCert_clicked();

private:
    Ui::EditDialog* ui;
    std::vector<win_cert_st> winCerts;
    StoredServer* ss;
};

