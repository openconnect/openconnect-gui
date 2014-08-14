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

int StoredServer::load(QString &name)
{
    settings->beginGroup(PREFIX+name);
    this->username = settings->value("username").toString();
    this->groupname = settings->value("groupname").toString();
    this->password = settings->value("password").toString();

    this->server_cert = settings->value("server-cert").toByteArray();
    this->user_cert = settings->value("user-cert").toByteArray();

    settings->endGroup();
    return 0;
}

void StoredServer::set_server_cert(gnutls_datum_t *cert)
{
    this->server_cert.setRawData((char*)cert->data, cert->size);
}

void StoredServer::set_user_cert(gnutls_datum_t *cert)
{
    this->user_cert.setRawData((char*)cert->data, cert->size);
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
    settings->setValue("user-cert", this->user_cert);

    settings->endGroup();
    return 0;
}

