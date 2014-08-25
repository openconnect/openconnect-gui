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


#include <QInputDialog>
#include <QString>
#include "keypair.h"
#include <gnutls/pkcs12.h>

KeyPair::KeyPair()
{
}

KeyPair::~KeyPair()
{
}

static
int load_pkcs12_file(QWidget *w, Key &key, Cert &cert, QString File)
{
    gnutls_datum_t raw = {NULL, 0};
    int ret;
    gnutls_pkcs12_t pkcs12 = NULL;
    bool ok;
    QString text, pass;
    int pem = 0;
    char *p;
    gnutls_x509_privkey_t xkey;
    gnutls_x509_crt_t *xcert;
    unsigned int xcert_size;
    unsigned i;

    if (w == NULL || File.startsWith("pkcs11:") || File.startsWith("tpmkey:")) {
        return -1;
    }

    ret = gnutls_load_file(File.toAscii().data(), &raw);
    if (ret < 0) {
        goto fail;
    }

    /* check if the file data contain BEGIN PKCS12 */
    p = strstr((char*)raw.data, "--- BEGIN ");
    if (p != NULL) {
        pem = 1;
        if (strstr(p, "--- BEGIN PKCS12") == 0)
            return -1;
    }

    ret = gnutls_pkcs12_init(&pkcs12);
    if (ret < 0) {
        goto fail;
    }

    ret = gnutls_pkcs12_import(pkcs12, &raw, (pem!=0)?GNUTLS_X509_FMT_PEM:GNUTLS_X509_FMT_DER, 0);
    if (ret < 0)
        goto fail;

    pass = QInputDialog::getText(w, QLatin1String("This file requires a password"),
                                    QLatin1String("Please enter your password"), QLineEdit::Normal,
                                    QString(), &ok);
    if (!ok)
        goto fail;

    ret = gnutls_pkcs12_verify_mac(pkcs12, pass.toAscii().data());
    if (ret < 0)
        goto fail;


    ret = gnutls_pkcs12_simple_parse(pkcs12, pass.toAscii().data(), &xkey, &xcert, &xcert_size, NULL, NULL, NULL, 0);
    if (ret < 0)
        goto fail;

    if (xkey)
        key.set(xkey);
    if (xcert_size > 0)
        cert.set(xcert[0]);

    for (i=1;i<xcert_size;i++)
        gnutls_x509_crt_deinit(xcert[i]);
    ret = 0;
    goto cleanup;

 fail:
    ret = -1;
 cleanup:
    gnutls_free(raw.data);
    if (pkcs12)
        gnutls_pkcs12_deinit(pkcs12);
    return ret;
}

int KeyPair::import(QString File)
{
int ret1 = 0, ret2 = 0;

    ret1 = this->key.import(File);
    if (ret1 != 0) {
        last_err = key.last_err;
    } else {
        std::cerr << "Key imported\n";
    }

    ret2 = this->cert.import(File);
    if (ret2 != 0) {
        last_err = cert.last_err;
    } else {
        std::cerr << "Certificate imported\n";
    }

    if (ret1 != 0 && ret2 != 0)
        goto fallback;
    return 0;

fallback:
    /* try PKCS #12 last */
    return load_pkcs12_file(this->w, this->key, this->cert, File);
}

int KeyPair::cert_export(QByteArray &data)
{
    return cert.data_export(data);
}

int KeyPair::key_export(QByteArray &data)
{
    return key.data_export(data);
}
