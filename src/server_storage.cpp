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

#include "server_storage.h"
#include "cryptdata.h"
#include <QSettings>
#include <cstdio>

StoredServer::~StoredServer(void)
{
}

StoredServer::StoredServer()
    : m_server_hash_algo(0)
{
    set_window(nullptr);
}

// LCA: drop thsi define from whole project...
#define PREFIX "server:"

void StoredServer::clear_password()
{
    this->m_password.clear();
}

void StoredServer::clear_groupname()
{
    this->m_groupname.clear();
}

void StoredServer::clear_cert()
{
    this->m_client.cert.clear();
}

void StoredServer::clear_key()
{
    this->m_client.key.clear();
}

void StoredServer::clear_ca()
{
    this->m_ca_cert.clear();
}

void StoredServer::clear_server_hash()
{
    this->m_server_hash.clear();
    this->m_server_hash_algo = 0;
}

QString StoredServer::get_cert_file()
{
    QString File;
    if (this->m_client.cert.is_ok()) {
        this->m_client.cert.tmpfile_export(File);
    }
    return File;
}

QString StoredServer::get_key_file()
{
    QString File;
    if (this->m_client.key.is_ok()) {
        this->m_client.key.tmpfile_export(File);
    }
    return File;
}

QString StoredServer::get_key_url() const
{
    QString File;
    if (this->m_client.key.is_ok()) {
        this->m_client.key.get_url(File);
    }
    return File;
}

QString StoredServer::get_ca_cert_file()
{
    QString File;
    if (this->m_ca_cert.is_ok()) {
        this->m_ca_cert.tmpfile_export(File);
    }
    return File;
}

int StoredServer::set_ca_cert(const QString& filename)
{
    int ret = this->m_ca_cert.import_file(filename);
    this->m_last_err = this->m_ca_cert.last_err;
    return ret;
}

int StoredServer::set_client_cert(const QString& filename)
{
    int ret = this->m_client.import_cert(filename);
    this->m_last_err = this->m_client.last_err;

    if (ret != 0) {
        ret = this->m_client.import_pfx(filename);
        this->m_last_err = this->m_client.last_err;
    }
    return ret;
}

int StoredServer::set_client_key(const QString& filename)
{
    int ret = this->m_client.import_key(filename);
    this->m_last_err = this->m_client.last_err;
    return ret;
}

void StoredServer::get_server_hash(QString& hash) const
{
    if (this->m_server_hash_algo == 0) {
        hash = "";
    } else {
        hash = gnutls_mac_get_name((gnutls_mac_algorithm_t)this->m_server_hash_algo);
        hash += ":";
        hash += this->m_server_hash.toHex();
    }
}

int StoredServer::load(QString& name)
{
    this->m_label = name;
    QSettings settings;
    settings.beginGroup(PREFIX + name);

    this->m_servername = settings.value("server").toString();
    if (this->m_servername.isEmpty() == true) {
        this->m_servername = name;
    }

    this->m_username = settings.value("username").toString();
    this->m_batch_mode = settings.value("batch").toBool();
    this->m_proxy = settings.value("proxy").toBool();
    this->m_disable_udp = settings.value("disable-udp").toBool();
    this->m_minimize_on_connect = settings.value("minimize-on-connect").toBool();
    this->m_reconnect_timeout = settings.value("reconnect-timeout", 300).toInt();
    this->m_dtls_attempt_period = settings.value("dtls_attempt_period", 25).toInt();

    bool ret = false;
    int rval = 0;

    if (this->m_batch_mode == true) {
        this->m_groupname = settings.value("groupname").toString();
        ret = CryptData::decode(this->m_servername,
            settings.value("password").toByteArray(),
            this->m_password);
        if (ret == false)
            rval = -1;
    }

    QByteArray data;
    data = settings.value("ca-cert").toByteArray();
    if (data.isEmpty() == false && this->m_ca_cert.import_pem(data) < 0) {
        this->m_last_err = this->m_ca_cert.last_err;
        rval = -1;
    }

    data = settings.value("client-cert").toByteArray();
    if (data.isEmpty() == false && this->m_client.cert.import_pem(data) < 0) {
        this->m_last_err = this->m_client.cert.last_err;
        rval = -1;
    }

    QString str;
    ret = CryptData::decode(this->m_servername,
        settings.value("client-key").toByteArray(), str);
    if (ret == false) {
        rval = -1;
    }

    if (is_url(str) == true) {
        this->m_client.key.import_file(str);
    } else {
        data = str.toLatin1();
        this->m_client.key.import_pem(data);
    }

    this->m_server_hash = settings.value("server-hash").toByteArray();
    this->m_server_hash_algo = settings.value("server-hash-algo").toInt();

    ret = CryptData::decode(this->m_servername,
        settings.value("token-str").toByteArray(),
        this->m_token_string);
    if (ret == false) {
        rval = -1;
    }

    this->m_token_type = settings.value("token-type").toInt();

    settings.endGroup();
    return rval;
}

int StoredServer::save()
{
    QSettings settings;
    settings.beginGroup(PREFIX + this->m_label);
    settings.setValue("server", this->m_servername);
    settings.setValue("batch", this->m_batch_mode);
    settings.setValue("proxy", this->m_proxy);
    settings.setValue("disable-udp", this->m_disable_udp);
    settings.setValue("minimize-on-connect", this->m_minimize_on_connect);
    settings.setValue("reconnect-timeout", this->m_reconnect_timeout);
    settings.setValue("dtls_attempt_period", this->m_dtls_attempt_period);
    settings.setValue("username", this->m_username);

    if (this->m_batch_mode == true) {
        settings.setValue("password",
            CryptData::encode(this->m_servername, this->m_password));
        settings.setValue("groupname", this->m_groupname);
    }

    QByteArray data;
    this->m_ca_cert.data_export(data);
    settings.setValue("ca-cert", data);

    this->m_client.cert_export(data);
    settings.setValue("client-cert", data);

    this->m_client.key_export(data);
    QString str = QString::fromLatin1(data);
    settings.setValue("client-key", CryptData::encode(this->m_servername, str));

    settings.setValue("server-hash", this->m_server_hash);
    settings.setValue("server-hash-algo", this->m_server_hash_algo);

    settings.setValue("token-str",
        CryptData::encode(this->m_servername, this->m_token_string));
    settings.setValue("token-type", this->m_token_type);

    settings.endGroup();
    return 0;
}

const QString& StoredServer::get_username() const
{
    return this->m_username;
}

const QString& StoredServer::get_password() const
{
    return this->m_password;
}

const QString& StoredServer::get_groupname() const
{
    return this->m_groupname;
}

const QString& StoredServer::get_servername() const
{
    return this->m_servername;
}

const QString& StoredServer::get_label() const
{
    return this->m_label;
}

void StoredServer::set_username(const QString& username)
{
    this->m_username = username;
}

void StoredServer::set_password(const QString& password)
{
    this->m_password = password;
}

void StoredServer::set_groupname(const QString& groupname)
{
    this->m_groupname = groupname;
}

void StoredServer::set_servername(const QString& servername)
{
    this->m_servername = servername;
}

void StoredServer::set_label(const QString& label)
{
    this->m_label = label;
}

void StoredServer::set_disable_udp(bool v)
{
    this->m_disable_udp = v;
}

bool StoredServer::get_disable_udp() const
{
    return this->m_disable_udp;
}

QString StoredServer::get_client_cert_hash()
{
    return m_client.cert.sha1_hash();
}

QString StoredServer::get_ca_cert_hash()
{
    return m_ca_cert.sha1_hash();
}

void StoredServer::set_window(QWidget* w)
{
    m_client.set_window(w);
}

void StoredServer::set_batch_mode(const bool mode)
{
    this->m_batch_mode = mode;
}

bool StoredServer::get_batch_mode() const
{
    return this->m_batch_mode;
}

bool StoredServer::get_minimize() const
{
    return this->m_minimize_on_connect;
}

bool StoredServer::get_proxy() const
{
    return this->m_proxy;
}

bool StoredServer::client_is_complete() const
{
    return m_client.is_complete();
}

void StoredServer::set_minimize(const bool t)
{
    this->m_minimize_on_connect = t;
}

void StoredServer::set_proxy(const bool t)
{
    this->m_proxy = t;
}

int StoredServer::get_reconnect_timeout() const
{
    return m_reconnect_timeout;
}

void StoredServer::set_reconnect_timeout(const int timeout)
{
    m_reconnect_timeout = timeout;
}

int StoredServer::get_dtls_reconnect_timeout() const
{
    return m_dtls_attempt_period;
}

void StoredServer::set_dtls_reconnect_timeout(const int timeout)
{
    m_dtls_attempt_period = timeout;
}

QString StoredServer::get_token_str()
{
    return this->m_token_string;
}

void StoredServer::set_token_str(const QString& str)
{
    this->m_token_string = str;
}

int StoredServer::get_token_type()
{
    return this->m_token_type;
}

void StoredServer::set_token_type(const int type)
{
    this->m_token_type = type;
}

void StoredServer::set_server_hash(const unsigned algo, const QByteArray& hash)
{
    this->m_server_hash_algo = algo;
    this->m_server_hash = hash;
}

unsigned StoredServer::get_server_hash(QByteArray& hash) const
{
    hash = this->m_server_hash;
    return this->m_server_hash_algo;
}
