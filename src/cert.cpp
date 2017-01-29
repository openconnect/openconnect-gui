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

#include "cert.h"
#include "common.h"
extern "C" {
#include <gnutls/pkcs11.h>
}
#include <cstdio>

Cert::Cert()
    : crt(nullptr)
    , imported(false)
{
}

Cert::~Cert()
{
    clear();
}

void Cert::clear()
{
    if (this->crt) {
        gnutls_x509_crt_deinit(crt);
        crt = nullptr;
        imported = false;
    }
}

static int import_cert(gnutls_x509_crt_t* crt, gnutls_datum_t* raw,
    unsigned pem)
{
    if (raw->size == 0) {
        return -1;
    }

    gnutls_x509_crt_init(crt);

    int ret = gnutls_x509_crt_import(*crt, raw, GNUTLS_X509_FMT_PEM);
    if (pem == 0
        && (ret == GNUTLS_E_BASE64_DECODING_ERROR || ret == GNUTLS_E_BASE64_UNEXPECTED_HEADER_ERROR)) {
        ret = gnutls_x509_crt_import(*crt, raw, GNUTLS_X509_FMT_DER);
    }
    if (ret < 0) {
        goto fail;
    }
    return 0;
fail:
    gnutls_x509_crt_deinit(*crt);
    *crt = nullptr;
    return ret;
}

int Cert::import_pem(const QByteArray& data)
{
    if (this->imported != false) {
        this->clear();
    }

    gnutls_datum_t raw;
    raw.data = (unsigned char*)data.constData();
    raw.size = data.size();

    int ret = import_cert(&this->crt, &raw, 1);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    this->imported = true;
    return 0;
}

void Cert::set(gnutls_x509_crt_t crt)
{
    clear();
    this->crt = crt;
    this->imported = true;
}

int Cert::data_export(QByteArray& data)
{
    data.clear();

    if (this->imported != true) {
        return -1;
    }

    gnutls_datum_t raw;
    int ret = gnutls_x509_crt_export2(this->crt, GNUTLS_X509_FMT_PEM, &raw);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    data = QByteArray((char*)raw.data, raw.size);
    gnutls_free(raw.data);
    return 0;
}

int Cert::import_file(const QString& File)
{
    if (File.isEmpty() == true) {
        return -1;
    }

    if (this->imported != false) {
        this->clear();
    }

    if (is_url(File)) {
        gnutls_x509_crt_init(&this->crt);

        int ret = gnutls_x509_crt_import_pkcs11_url(this->crt, File.toLatin1().data(), 0);
        if (ret < 0) {
            ret = gnutls_x509_crt_import_pkcs11_url(this->crt,
                File.toLatin1().data(),
                GNUTLS_PKCS11_OBJ_FLAG_LOGIN);
        }

        if (ret < 0) {
            this->last_err = gnutls_strerror(ret);
            return -1;
        }
        this->imported = true;
        return 0;
    }

    /* normal file */
    gnutls_datum_t contents = { nullptr, 0 };
    int ret = gnutls_load_file(File.toLatin1().data(), &contents);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    ret = import_cert(&this->crt, &contents, 0);
    gnutls_free(contents.data);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    this->imported = true;
    return 0;
}

int Cert::tmpfile_export(QString& filename)
{
    gnutls_datum_t out;
    int ret = gnutls_x509_crt_export2(this->crt, GNUTLS_X509_FMT_PEM, &out);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    QByteArray qa;
    qa.append((const char*)out.data, out.size);
    gnutls_free(out.data);

    tmpfile.resize(0);
    filename = TMP_CERT_PREFIX;
    tmpfile.setFileTemplate(filename);

    tmpfile.open();
    ret = tmpfile.write(qa);
    tmpfile.close();
    if (ret == -1) {
        return -1;
    }
    filename = tmpfile.fileName();

    return 0;
}

QString Cert::sha1_hash()
{
    if (imported == false) {
        return "";
    }

    unsigned char id[32];
    size_t len = sizeof(id);
    int ret = gnutls_x509_crt_get_key_id(this->crt, 0, id, &len);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return "";
    }

    QByteArray array;
    array.append((const char*)id, len);
    QByteArray hex = array.toHex();
    QString s = QObject::tr("SHA1:") + hex;
    return s;
}

bool Cert::is_ok() const
{
    return imported;
}
