#ifndef STORAGE_H
#define STORAGE_H

#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QSettings>
#include <gnutls/gnutls.h>

QStringList get_server_list(QSettings *settings);

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

    const gnutls_datum_t get_user_cert(void) {
        gnutls_datum_t t = {(unsigned char*)user_cert.data(), (unsigned)user_cert.size()};
        return t;
    }

    void set_servername(QString & name) {
        this->servername = name;
    }

    void set_username(QString & username) {
        this->username = username;
    }

    void set_password(QString & p) {
        this->password = p;
    }

    void set_groupname(QString & groupname) {
        this->groupname = groupname;
    }

    void set_server_cert(gnutls_datum_t *cert);
    void set_user_cert(gnutls_datum_t *cert);
    int save();

private:
    QString username;
    QString password;
    QString groupname;
    QString servername;
    QByteArray server_cert;
    QByteArray user_cert;
    QSettings *settings;
};

#endif // STORAGE_H
