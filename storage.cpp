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

#include <storage.h>
#include <stdio.h>
#include <cryptdata.h>

StoredServer::~StoredServer(void)
{

}

StoredServer::StoredServer(QSettings *settings)
{
    this->server_hash_algo = 0;
    this->settings = settings;
};

#define PREFIX "server:"
QStringList get_server_list(QSettings *settings)
{
    QStringList keys = settings->allKeys();
    QString str;
    QStringList res;
    QString prefix = PREFIX;

    for (int i = 0; i < keys.size(); i++) {
        if (keys.at(i).startsWith(PREFIX) && keys.at(i).endsWith("/server")) {
            str = keys.at(i);
            str.remove(0, sizeof(PREFIX)-1); /* remove prefix */
            str.remove(str.size()-7, 7); /* remove /server suffix */
            res.append(str);
         }
    }
    return res;
}

void remove_server(QSettings *settings, QString server)
{
    QStringList keys = settings->allKeys();
    QString str;
    QStringList res;
    QString prefix = PREFIX;

    for (int i = 0; i < keys.size(); i++) {
        if (keys.at(i).startsWith(prefix+server)) {
            settings->remove(keys.at(i));
         }
    }
    return;
}

void StoredServer::clear_password()
{
    this->password.clear();
}

void StoredServer::clear_groupname()
{
    this->groupname.clear();
}

void StoredServer::clear_cert()
{
    this->client.cert.clear();
}

void StoredServer::clear_key()
{
    this->client.key.clear();
}

void StoredServer::clear_ca()
{
    this->ca_cert.clear();
}

void StoredServer::clear_server_hash()
{
    this->server_hash.clear();
    this->server_hash_algo = 0;
}

QString StoredServer::get_cert_file()
{
    QString File;
    if (this->client.cert.is_ok()) {
        this->client.cert.tmpfile_export(File);
    }
    return File;

}

QString StoredServer::get_key_file()
{
    QString File;
    if (this->client.key.is_ok()) {
        this->client.key.tmpfile_export(File);
    }
    return File;
}

QString StoredServer::get_key_url()
{
    QString File;
    if (this->client.key.is_ok()) {
        this->client.key.get_url(File);
    }
    return File;
}

QString StoredServer::get_ca_cert_file()
{
    QString File;
    if (this->ca_cert.is_ok()) {
        this->ca_cert.tmpfile_export(File);
    }
    return File;
}

int StoredServer::set_ca_cert(QString filename)
{
    int ret = this->ca_cert.import_file(filename);
    this->last_err = this->ca_cert.last_err;
    return ret;
}

int StoredServer::set_client_cert(QString filename)
{
    int ret = this->client.import_cert(filename);
    this->last_err = this->client.last_err;

    if (ret != 0 && this->client.key.is_ok() == false) {
       ret = this->client.import_pfx(filename);
       this->last_err = this->client.last_err;
    }
    return ret;
}

int StoredServer::set_client_key(QString filename)
{
    int ret = this->client.import_key(filename);
    this->last_err = this->client.last_err;
    return ret;
}

void StoredServer::get_server_hash(QString & hash)
{
    if (this->server_hash_algo == 0) {
        hash = "";
    } else {
        hash = gnutls_mac_get_name((gnutls_mac_algorithm_t)this->server_hash_algo);
        hash += ":";
        hash += this->server_hash.toHex();
    }
}

int StoredServer::load(QString &name)
{
    QByteArray data;
    QString str;
    bool ret;
    int rval = 0;

    this->label = name;
    settings->beginGroup(PREFIX+name);

    this->servername = settings->value("server").toString();
    if (this->servername.isEmpty() == true)
        this->servername = name;

    this->username = settings->value("username").toString();
    this->batch_mode = settings->value("batch").toBool();
    this->proxy = settings->value("proxy").toBool();
    this->disable_udp = settings->value("disable-udp").toBool();
    this->minimize_on_connect = settings->value("minimize-on-connect").toBool();

    if (this->batch_mode == true) {
        this->groupname = settings->value("groupname").toString();
        ret = CryptData::decode(this->servername, settings->value("password").toByteArray(), this->password);
        if (ret == false)
            rval = -1;
    }

    data = settings->value("ca-cert").toByteArray();
    if (data.isEmpty() == false && this->ca_cert.import_pem(data) < 0) {
        this->last_err = this->ca_cert.last_err;
        rval = -1;
    }

    data = settings->value("client-cert").toByteArray();
    if (data.isEmpty() == false && this->client.cert.import_pem(data) < 0) {
        this->last_err = this->client.cert.last_err;
        rval = -1;
    }

    ret = CryptData::decode(this->servername, settings->value("client-key").toByteArray(), str);
    if (ret == false)
        rval = -1;

    if (is_url(str) == true) {
        this->client.key.import_file(str);
    } else {
        data = str.toLatin1();
        this->client.key.import_pem(data);
    }

    this->server_hash = settings->value("server-hash").toByteArray();
    this->server_hash_algo = settings->value("server-hash-algo").toInt();

    ret = CryptData::decode(this->servername, settings->value("token-str").toByteArray(), this->token_str);
    if (ret == false)
        rval = -1;

    this->token_type = settings->value("token-type").toInt();

    settings->endGroup();
    return rval;
}

int StoredServer::save()
{
    QString empty = "";
    QString str;
    QByteArray data;

    settings->beginGroup(PREFIX+this->label);
    settings->setValue("server", this->servername);
    settings->setValue("batch", this->batch_mode);
    settings->setValue("proxy", this->proxy);
    settings->setValue("disable-udp", this->disable_udp);
    settings->setValue("minimize-on-connect", this->minimize_on_connect);
    settings->setValue("username", this->username);

    if (this->batch_mode == true) {
        settings->setValue("password", CryptData::encode(this->servername, this->password));
        settings->setValue("groupname", this->groupname);
    }

    this->ca_cert.data_export(data);
    settings->setValue("ca-cert", data);

    this->client.cert_export(data);
    settings->setValue("client-cert", data);

    this->client.key_export(data);
    str = QString::fromLatin1(data);
    settings->setValue("client-key", CryptData::encode(this->servername, str));

    settings->setValue("server-hash", this->server_hash);
    settings->setValue("server-hash-algo", this->server_hash_algo);

    settings->setValue("token-str", CryptData::encode(this->servername, this->token_str));
    settings->setValue("token-type", this->token_type);

    settings->endGroup();
    return 0;
}

