/*
 * Copyright (C) 2014 Red Hat
 *
 * This file is part of openconnect-gui.
 *
 * openconnect-gui is free software: you can redistribute it and/or modify
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

#ifndef KEYPAIR_H
#define KEYPAIR_H

#include "key.h"
#include "cert.h"

class KeyPair
{
public:
    KeyPair();
    ~KeyPair();

    /* functions return zero on success */
    int import(QString File);
    void set_window(QWidget * w) {
        this->w = w;
        key.set_window(w);
    };

    int cert_export(QByteArray &data);
    int key_export(QByteArray &data);
    bool is_complete() {
        if (key.is_ok() == cert.is_ok())
            return true;
        return false;
    };

    QString last_err;

    Key key;
    Cert cert;
private:
    QWidget * w;

};

#endif // KEYPAIR_H
