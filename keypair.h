#ifndef KEYPAIR_H
#define KEYPAIR_H

#include "key.h"
#include "cert.h"

class KeyPair
{
public:
    KeyPair();
    ~KeyPair();

    /* functions return zero on success */
    int import(QString File);
    void set_window(QWidget * w) {
        this->w = w;
        key.set_window(w);
    };

    int cert_export(QByteArray &data);
    int key_export(QByteArray &data);
    bool is_complete() {
        if (key.is_ok() == cert.is_ok())
            return true;
        return false;
    };

    QString last_err;

    Key key;
    Cert cert;
private:
    QWidget * w;

};

#endif // KEYPAIR_H
