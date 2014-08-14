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

    const gnutls_datum_t get_server_cert(void) {
        gnutls_datum_t t = {(unsigned char*)server_cert.data(), (unsigned)server_cert.size()};
        return t;
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

    void set_server_cert(gnutls_datum_t *cert);
    int save();

private:
    QString username;
    QString password;
    QString groupname;
    QString servername;
    QByteArray server_cert;
    QSettings *settings;
};

#endif // STORAGE_H
