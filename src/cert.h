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

#include <QTemporaryFile>
extern "C" {
#include <gnutls/x509.h>
}

class Cert {
public:
    Cert();
    ~Cert();

    /* functions return zero on success */
    int import_file(const QString& File);
    int import_pem(const QByteArray& data);
    void set(gnutls_x509_crt_t crt);
    int data_export(QByteArray& data);
    int tmpfile_export(QString& File);
    QString sha1_hash();

    bool is_ok() const;
    void clear();

    QString last_err;

private:
    gnutls_x509_crt_t crt;
    QTemporaryFile tmpfile;
    bool imported;
};
