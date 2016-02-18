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

#ifndef STORAGE_H
#define STORAGE_H

#include "keypair.h"

class QSettings;

QStringList get_server_list(QSettings* settings);
void remove_server(QSettings* settings, QString server);

class StoredServer {
public:
    explicit StoredServer(QSettings* settings);
    ~StoredServer();

    int load(QString& name);

    QString& get_username(void);
    QString& get_password(void);
    QString& get_groupname(void);
    QString& get_servername(void);
    QString& get_label(void);

    void set_servername(QString name);
    void set_label(QString name);
    void set_username(QString username);
    void set_password(QString p);
    void set_groupname(QString& groupname);
    void set_disable_udp(bool v);

    QString get_cert_file();
    QString get_key_file();
    QString get_key_url();
    QString get_ca_cert_file();
    void clear_cert();
    void clear_key();
    void clear_ca();
    void clear_password();
    void clear_groupname();
    void clear_server_hash();

    QString get_client_cert_hash();

    QString get_ca_cert_hash();

    void set_window(QWidget* w);

    int set_ca_cert(QString filename);
    int set_client_cert(QString filename);
    int set_client_key(QString filename);
    void set_batch_mode(bool mode);
    bool get_batch_mode();

    bool get_minimize();
    bool get_proxy();
    bool client_is_complete();
    void set_minimize(bool t);
    void set_proxy(bool t);
    QString get_token_str();
    void set_token_str(QString str);
    int get_token_type();
    bool get_disable_udp();
    void set_token_type(int type);
    void set_server_hash(unsigned algo, QByteArray& hash);
    unsigned get_server_hash(QByteArray& hash);
    void get_server_hash(QString& hash);
    int save();

    QString last_err;

private:
    bool batch_mode;
    bool minimize_on_connect;
    bool proxy;
    bool disable_udp;
    QString username;
    QString password;
    QString groupname;
    QString servername;
    QString token_str;
    QString label;
    int token_type;
    QByteArray server_hash;
    unsigned server_hash_algo;
    Cert ca_cert;
    KeyPair client;
    QSettings* settings;
};

#endif // STORAGE_H
