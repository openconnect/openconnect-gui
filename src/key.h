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

#ifndef KEY_H
#define KEY_H

#include <QByteArray>
#include <QString>
#include <QTemporaryFile>
#include <gnutls/x509.h>

class Key {
public:
    /* functions return zero on success */
    int import_file(QString& File);
    int import_pem(QByteArray& data);
    void set(gnutls_x509_privkey_t privkey);
    ;

    void set_window(QWidget* w);
    ;

    int data_export(QByteArray& data);
    int tmpfile_export(QString& File);

    bool is_ok();
    void get_url(QString& url);

    void clear();

    Key();
    ~Key();
    QString last_err;

private:
    gnutls_x509_privkey_t privkey;
    QTemporaryFile tmpfile;
    QString url;
    QWidget* w;
    bool imported;
};

#endif // KEY_H
