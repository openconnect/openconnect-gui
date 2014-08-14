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

#include <storage.h>
#include <stdio.h>
StoredServer::~StoredServer(void)
{

}

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

int StoredServer::load(QString &name)
{
    this->servername = name;
    settings->beginGroup(PREFIX+name);
    this->username = settings->value("username").toString();
    this->groupname = settings->value("groupname").toString();
    this->password = settings->value("password").toString();

    this->server_cert = settings->value("server-cert").toByteArray();

    settings->endGroup();
    return 0;
}

void StoredServer::set_server_cert(gnutls_datum_t *cert)
{
    this->server_cert.setRawData((char*)cert->data, cert->size);
}

int StoredServer::save()
{
    QString empty = "";

    settings->beginGroup(PREFIX+this->servername);
    settings->setValue("server", this->servername);
    settings->setValue("username", this->username);
    settings->setValue("password", this->password);
    settings->setValue("groupname", this->groupname);
    settings->setValue("server-cert", this->server_cert);

    settings->endGroup();
    return 0;
}

