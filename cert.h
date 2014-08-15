#ifndef CERT_H
#define CERT_H

#include <QString>
#include <QByteArray>
#include <QTemporaryFile>
#include <gnutls/x509.h>
#include "common.h"

class Cert
{

public:
    /* functions return zero on success */
    int import(QString File);
    int import(QByteArray data);
    void set(gnutls_x509_crt_t crt) {
        this->crt = crt;
    };

    int data_export(QByteArray &data);
    int tmpfile_export(QString &File);
    QString sha1_hash(void);

    Cert();
    ~Cert();
    QString last_err;
private:
    gnutls_x509_crt_t crt;
    QTemporaryFile tmpfile;
};

#endif // CERT_H
