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

#include "cert.h"
#include "key.h"

class KeyPair {
public:
    KeyPair();
    ~KeyPair();

    /* functions return zero on success */
    int import_pfx(const QString& File);
    int import_cert(const QString& File);
    int import_key(const QString& File);
    void set_window(QWidget* w);

    int cert_export(QByteArray& data);
    int key_export(QByteArray& data);

    const bool is_complete() const;

    QString last_err;

    Key key;
    Cert cert;

private:
    QWidget* w;
};
