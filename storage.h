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

#ifndef STORAGE_H
#define STORAGE_H

#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QSettings>
#include <gnutls/gnutls.h>
#include "keypair.h"

QStringList get_server_list(QSettings *settings);
void remove_server(QSettings *settings, QString server);

class StoredServer
{
public:
    explicit StoredServer(QSettings *settings) {
        this->settings = settings;
    };

    ~StoredServer();

    int load(QString & name);
    QString &get_username(void) {
        return this->username;
    }

    QString &get_password(void) {
        return this->password;
    }

    QString &get_groupname(void) {
        return this->groupname;
    }

    QString &get_servername(void) {
        return this->servername;
    }

    void set_servername(QString name) {
        this->servername = name;
    }

    void set_username(QString username) {
        this->username = username;
    }

    void set_password(QString p) {
        this->password = p;
    }

    void set_groupname(QString & groupname) {
        this->groupname = groupname;
    }

    QString get_cert_file();
    QString get_key_file();
    QString get_ca_cert_file();

    QString get_client_cert_hash() {
        return ca_cert.sha1_hash();
    };

    QString get_ca_cert_hash() {
        return client.cert.sha1_hash();
    };

    int set_ca_cert(QString filename);
    int set_client_cert(QString filename);
    int set_client_key(QString filename);
    int save();

    QString last_err;
private:
    QString username;
    QString password;
    QString groupname;
    QString servername;
    Cert ca_cert;
    KeyPair client;
    QSettings *settings;
};

#endif // STORAGE_H
