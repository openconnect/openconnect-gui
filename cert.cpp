#include "cert.h"
#include <QTemporaryFile>
#include <gnutls/pkcs11.h>
Cert::Cert()
{
    imported = false;
    crt = NULL;
}

Cert::~Cert()
{
    if (crt)
        gnutls_x509_crt_deinit(crt);
}

static int import_cert(gnutls_x509_crt_t *crt, gnutls_datum_t *raw)
{
    int ret;

    if (raw->size == 0)
        return -1;

    gnutls_x509_crt_init(crt);

    ret = gnutls_x509_crt_import(*crt, raw, GNUTLS_X509_FMT_PEM);
    if (ret == GNUTLS_E_BASE64_DECODING_ERROR || ret == GNUTLS_E_BASE64_UNEXPECTED_HEADER_ERROR)
        ret = gnutls_x509_crt_import(*crt, raw, GNUTLS_X509_FMT_DER);
    if (ret < 0) {
        goto fail;
    }
    return 0;
 fail:
    gnutls_x509_crt_deinit(*crt);
    *crt = NULL;
    return ret;
}

int Cert::import(QByteArray data)
{
    int ret;
    gnutls_datum_t raw;

    if (this->imported != false)
        return -1;

    raw.data = (unsigned char*)data.constData();
    raw.size = data.size();

    ret = import_cert(&this->crt, &raw);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    this->imported = true;
    return 0;
}

int Cert::data_export(QByteArray &data)
{
    int ret;
    gnutls_datum_t raw;

    if (this->imported != true)
        return -1;

    data.clear();
    ret = gnutls_x509_crt_export2(this->crt, GNUTLS_X509_FMT_PEM, &raw);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    data.append((char*)raw.data, raw.size);
    gnutls_free(raw.data);
    return 0;
}

int Cert::import(QString File)
{
    int ret;
    gnutls_datum_t contents = {NULL, 0};

    if (this->imported != false)
        return -1;

    if (File.startsWith("pkcs11:")) {
        ret = gnutls_x509_crt_import_pkcs11_url(this->crt, File.toAscii().data(), 0);
        if (ret < 0)
            ret = gnutls_x509_crt_import_pkcs11_url(this->crt, File.toAscii().data(), GNUTLS_PKCS11_OBJ_FLAG_LOGIN);

        if (ret < 0) {
            this->last_err = gnutls_strerror(ret);
            return -1;
        }
        this->imported = true;
        return 0;
    }

    /* normal file */
    ret = gnutls_load_file(File.toAscii().data(), &contents);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    ret = import_cert(&this->crt, &contents);
    gnutls_free(contents.data);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    this->imported = true;
    return 0;
}

int Cert::tmpfile_export(QString &filename)
{
    int ret;
    gnutls_datum_t out;
    QByteArray qa;

    tmpfile.resize(0);
    filename = "certXXXXXX";

    tmpfile.setFileTemplate(filename);
    ret = gnutls_x509_crt_export2(this->crt, GNUTLS_X509_FMT_PEM, &out);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    qa.append((const char*)out.data, out.size);
    gnutls_free(out.data);

    tmpfile.open();
    ret = tmpfile.write(qa);
    tmpfile.close();
    if (ret == -1) {
        return -1;
    }
    filename = tmpfile.fileName();

    return 0;
}

#include <stdio.h>
QString Cert::sha1_hash(void)
{
    unsigned char id[32];
    int ret;
    size_t len;
    QString s;
    QByteArray array, hex;

    if (imported == false)
        return "";

    len = sizeof(id);
    ret = gnutls_x509_crt_get_key_id(this->crt, 0, id, &len);

    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return "";
    }

    array.append((const char*)id, len);
    hex = array.toHex();
    s = hex;
    return s;
}
