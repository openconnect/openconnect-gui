#ifndef KEY_H
#define KEY_H

#include <QString>
#include <QByteArray>
#include <QTemporaryFile>
#include "mainwindow.h"
#include <gnutls/x509.h>

class Key
{
public:
    /* functions return zero on success */
    int import(QString File);
    int import(QByteArray data);
    void set(gnutls_x509_privkey_t privkey) {
        this->privkey = privkey;
    };

    void set_window(MainWindow * w) {
        this->w = w;
    };

    int data_export(QByteArray &data);
    int tmpfile_export(QString &File);

    Key();
    ~Key();
    QString last_err;
private:
    gnutls_x509_privkey_t privkey;
    QTemporaryFile tmpfile;
    QString url;
    MainWindow *w;
};


#endif // KEY_H
