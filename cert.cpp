#include "cert.h"
#include <QTemporaryFile>
#include <gnutls/pkcs11.h>

Cert::Cert()
{
    gnutls_x509_crt_init(&this->crt);
}

Cert::~Cert()
{
    gnutls_x509_crt_deinit(this->crt);
}

static int import_cert(gnutls_x509_crt_t crt, gnutls_datum_t *raw)
{
    int ret;
    ret = gnutls_x509_crt_import(crt, raw, GNUTLS_X509_FMT_PEM);
    if (ret == GNUTLS_E_BASE64_DECODING_ERROR || ret == GNUTLS_E_BASE64_UNEXPECTED_HEADER_ERROR)
        ret = gnutls_x509_crt_import(crt, raw, GNUTLS_X509_FMT_DER);
    if (ret < 0) {
        return ret;
    }
    return 0;
}

int Cert::import(QByteArray data)
{
    int ret;
    gnutls_datum_t raw;

    raw.data = (unsigned char*)data.constData();
    raw.size = data.size();

    ret = import_cert(this->crt, &raw);
    gnutls_free(raw.data);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }
    return 0;
}

int Cert::data_export(QByteArray &data)
{
    int ret;
    gnutls_datum_t raw;

    data.clear();
    ret = gnutls_x509_crt_export2(this->crt, GNUTLS_X509_FMT_PEM, &raw);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    data.setRawData((char*)raw.data, raw.size);
    gnutls_free(raw.data);
    return 0;
}

int Cert::import(QString File)
{
    int ret;
    gnutls_datum_t contents = {NULL, 0};

    if (File.startsWith("pkcs11:")) {
        ret = gnutls_x509_crt_import_pkcs11_url(this->crt, File.toAscii().data(), 0);
        if (ret < 0)
            ret = gnutls_x509_crt_import_pkcs11_url(this->crt, File.toAscii().data(), GNUTLS_PKCS11_OBJ_FLAG_LOGIN);

        if (ret < 0) {
            this->last_err = gnutls_strerror(ret);
            return -1;
        }
        return 0;
    }

    /* normal file */
    ret = gnutls_load_file(File.toAscii().data(), &contents);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    ret = import_cert(this->crt, &contents);
    gnutls_free(contents.data);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }
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

    qa.fromRawData((const char*)out.data, out.size);
    gnutls_free(out.data);

    ret = tmpfile.write(qa);
    if (ret == -1) {
        return -1;
    }

    return 0;
}

QString Cert::sha1_hash(void)
{
    unsigned char id[32];
    char hex[64+1];
    int ret;
    gnutls_datum_t data;
    size_t len;
    QString s;

    len = sizeof(id);

    ret = gnutls_x509_crt_get_key_id(this->crt, 0, id, &len);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return NULL;
    }

    data.data = id;
    data.size = len;
    len = sizeof(hex);
    ret = gnutls_hex_encode(&data, hex, &len);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return NULL;
    }

    s = hex;
    return s;
}
