#include "key.h"
#include <QTemporaryFile>
#include <QInputDialog>
#include <gnutls/pkcs11.h>

Key::Key()
{
    imported = false;
    privkey = NULL;
}

Key::~Key()
{
    if (this->privkey)
        gnutls_x509_privkey_deinit(this->privkey);
}

static int import_Key(QWidget *w, gnutls_x509_privkey_t *privkey, gnutls_datum_t *raw)
{
    int ret;

    if (raw->size == 0)
        return -1;

    gnutls_x509_privkey_init(privkey);

    ret = gnutls_x509_privkey_import2(*privkey, raw, GNUTLS_X509_FMT_PEM, NULL, 0);
    if (ret == GNUTLS_E_DECRYPTION_FAILED && w != NULL) {
        bool ok;
        QString text;

        text = QInputDialog::getText(w, QLatin1String("This file requires a password"),
                                        QLatin1String("Please enter your password"), QLineEdit::Normal,
                                        QString(), &ok);
        if (!ok) {
            ret = -1;
            goto fail;
        }

        ret = gnutls_x509_privkey_import2(*privkey, raw, GNUTLS_X509_FMT_PEM, text.toAscii().data(), 0);
    }

    if (ret == GNUTLS_E_BASE64_DECODING_ERROR || ret == GNUTLS_E_BASE64_UNEXPECTED_HEADER_ERROR)
        ret = gnutls_x509_privkey_import(*privkey, raw, GNUTLS_X509_FMT_DER);
    if (ret < 0) {
        goto fail;
    }

    return 0;
 fail:
    gnutls_x509_privkey_deinit(*privkey);
    *privkey = NULL;
    return ret;
}

int Key::import(QByteArray data)
{
    int ret;
    gnutls_datum_t raw;

    raw.data = (unsigned char*)data.constData();
    raw.size = data.size();

    ret = import_Key(this->w, &this->privkey, &raw);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }
    imported = true;
    return 0;
}

int Key::data_export(QByteArray &data)
{
    int ret;
    gnutls_datum_t raw;

    if (imported == false)
        return -1;

    data.clear();
    if (this->url.isEmpty() == false) {
        /* export the URL */
        data.append(this->url);
        return 0;
    }

    ret = gnutls_x509_privkey_export2(this->privkey, GNUTLS_X509_FMT_PEM, &raw);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }
    data.append((char*)raw.data, raw.size);
    gnutls_free(raw.data);
    return 0;
}

int Key::import(QString File)
{
    int ret;
    gnutls_datum_t contents = {NULL, 0};

    if (File.startsWith("pkcs11:") || File.startsWith("tpmkey:")) {
        this->url = File;
        imported = true;
        return 0;
    }

    /* normal file */
    ret = gnutls_load_file(File.toAscii().data(), &contents);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    ret = import_Key(this->w, &this->privkey, &contents);
    gnutls_free(contents.data);
    if (ret < 0) {
        this->last_err = gnutls_strerror(ret);
        return -1;
    }

    imported = true;
    return 0;
}

int Key::tmpfile_export(QString &filename)
{
    int ret;
    gnutls_datum_t out;
    QByteArray qa;

    if (this->imported == false)
        return -1;

    if (this->url.isEmpty() == false) {
        filename = this->url;
        return 0;
    }

    tmpfile.resize(0);
    filename = "keyXXXXXX";

    tmpfile.setFileTemplate(filename);

    ret = gnutls_x509_privkey_export2(this->privkey, GNUTLS_X509_FMT_PEM, &out);
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
