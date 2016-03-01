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

#include "storage.h"
#include "cryptdata.h"
#include <QSettings>
#include <cstdio>

StoredServer::~StoredServer(void)
{
}

StoredServer::StoredServer(QSettings* settings) :
    m_server_hash_algo(0),
    m_settings(settings)
{
    set_window(nullptr);
}

#define PREFIX "server:"
QStringList get_server_list(QSettings* settings)
{
    QStringList res;
    for (const auto& key : settings->allKeys()) {
        if (key.startsWith(PREFIX) && key.endsWith("/server")) {
            QString str{key};
            str.remove(0, sizeof(PREFIX) - 1); /* remove prefix */
            str.remove(str.size() - 7, 7); /* remove /server suffix */
            res.append(str);
        }
    }
    return res;
}

void remove_server(QSettings* settings, const QString& server)
{
    QString prefix = PREFIX;
    for (const auto& key : settings->allKeys()) {
        if (key.startsWith(prefix + server)) {
            settings->remove(key);
        }
    }
}

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
        hash = gnutls_mac_get_name((gnutls_mac_algorithm_t) this->m_server_hash_algo);
        hash += ":";
        hash += this->m_server_hash.toHex();
    }
}

int StoredServer::load(QString& name)
{
    this->m_label = name;
    m_settings->beginGroup(PREFIX + name);

    this->m_servername = m_settings->value("server").toString();
    if (this->m_servername.isEmpty() == true) {
        this->m_servername = name;
    }

    this->m_username = m_settings->value("username").toString();
    this->m_batch_mode = m_settings->value("batch").toBool();
    this->m_proxy = m_settings->value("proxy").toBool();
    this->m_disable_udp = m_settings->value("disable-udp").toBool();
    this->m_minimize_on_connect = m_settings->value("minimize-on-connect").toBool();

    bool ret = false;
    int rval = 0;

    if (this->m_batch_mode == true) {
        this->m_groupname = m_settings->value("groupname").toString();
        ret = CryptData::decode(this->m_servername,
                                m_settings->value("password").toByteArray(),
                                this->m_password);
        if (ret == false)
            rval = -1;
    }

    QByteArray data;
    data = m_settings->value("ca-cert").toByteArray();
    if (data.isEmpty() == false && this->m_ca_cert.import_pem(data) < 0) {
        this->m_last_err = this->m_ca_cert.last_err;
        rval = -1;
    }

    data = m_settings->value("client-cert").toByteArray();
    if (data.isEmpty() == false && this->m_client.cert.import_pem(data) < 0) {
        this->m_last_err = this->m_client.cert.last_err;
        rval = -1;
    }

    QString str;
    ret = CryptData::decode(this->m_servername,
                            m_settings->value("client-key").toByteArray(), str);
    if (ret == false) {
        rval = -1;
    }

    if (is_url(str) == true) {
        this->m_client.key.import_file(str);
    } else {
        data = str.toLatin1();
        this->m_client.key.import_pem(data);
    }

    this->m_server_hash = m_settings->value("server-hash").toByteArray();
    this->m_server_hash_algo = m_settings->value("server-hash-algo").toInt();

    ret = CryptData::decode(this->m_servername,
                            m_settings->value("token-str").toByteArray(),
                            this->m_token_string);
    if (ret == false) {
        rval = -1;
    }

    this->m_token_type = m_settings->value("token-type").toInt();

    m_settings->endGroup();
    return rval;
}

int StoredServer::save()
{
    m_settings->beginGroup(PREFIX + this->m_label);
    m_settings->setValue("server", this->m_servername);
    m_settings->setValue("batch", this->m_batch_mode);
    m_settings->setValue("proxy", this->m_proxy);
    m_settings->setValue("disable-udp", this->m_disable_udp);
    m_settings->setValue("minimize-on-connect", this->m_minimize_on_connect);
    m_settings->setValue("username", this->m_username);

    if (this->m_batch_mode == true) {
        m_settings->setValue("password",
                           CryptData::encode(this->m_servername, this->m_password));
        m_settings->setValue("groupname", this->m_groupname);
    }

    QByteArray data;
    this->m_ca_cert.data_export(data);
    m_settings->setValue("ca-cert", data);

    this->m_client.cert_export(data);
    m_settings->setValue("client-cert", data);

    this->m_client.key_export(data);
    QString str = QString::fromLatin1(data);
    m_settings->setValue("client-key", CryptData::encode(this->m_servername, str));

    m_settings->setValue("server-hash", this->m_server_hash);
    m_settings->setValue("server-hash-algo", this->m_server_hash_algo);

    m_settings->setValue("token-str",
                       CryptData::encode(this->m_servername, this->m_token_string));
    m_settings->setValue("token-type", this->m_token_type);

    m_settings->endGroup();
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

const bool StoredServer::get_disable_udp() const
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

const bool StoredServer::get_batch_mode() const
{
    return this->m_batch_mode;
}

const bool StoredServer::get_minimize() const
{
    return this->m_minimize_on_connect;
}

const bool StoredServer::get_proxy() const
{
    return this->m_proxy;
}

const bool StoredServer::client_is_complete() const
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
