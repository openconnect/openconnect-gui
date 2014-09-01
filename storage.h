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
    StoredServer(QSettings *settings);
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
    void clear_cert();
    void clear_key();
    void clear_ca();
    void clear_password();
    void clear_groupname();
    void clear_server_hash();

    QString get_client_cert_hash() {
        return client.cert.sha1_hash();
    }

    QString get_ca_cert_hash() {
        return ca_cert.sha1_hash();
    }

    void set_window(QWidget * w) {
        client.key.set_window(w);
    }

    int set_ca_cert(QString filename);
    int set_client_cert(QString filename);
    int set_client_key(QString filename);
    void set_batch_mode(bool mode) {
        this->batch_mode = mode;
    }
    bool get_batch_mode() {
        return this->batch_mode;
    }

    bool get_minimize() {
        return this->minimize_on_connect;
    }

    bool get_proxy() {
        return this->proxy;
    }

    bool client_is_complete() {
        return client.is_complete();
    };

    void set_minimize(bool t) {
        this->minimize_on_connect = t;
    }

    void set_proxy(bool t) {
        this->proxy = t;
    }

    QString get_token_str() {
        return this->token_str;
    }

    void set_token_str(QString str) {
        this->token_str = str;
    }

    int get_token_type() {
        return this->token_type;
    }

    void set_token_type(int type) {
        this->token_type = type;
    }

    void set_server_hash(unsigned algo, QByteArray &hash) {
        this->server_hash_algo = algo;
        this->server_hash = hash;
    }

    unsigned get_server_hash(QByteArray & hash) {
        hash = this->server_hash;
        return this->server_hash_algo;
    }

    void get_server_hash(QString & hash);

    int save();

    QString last_err;
private:
    bool batch_mode;
    bool minimize_on_connect;
    bool proxy;
    QString username;
    QString password;
    QString groupname;
    QString servername;
    QString token_str;
    int token_type;
    QByteArray server_hash;
    unsigned server_hash_algo;
    Cert ca_cert;
    KeyPair client;
    QSettings *settings;
};

#endif // STORAGE_H
