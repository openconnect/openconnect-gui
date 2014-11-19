#include "cryptdata.h"
#include <QByteArray>

#if defined(_WIN32)
#include <windows.h>
#include <winbase.h>

typedef WINBOOL (WINAPI *CryptProtectDataFunc) (DATA_BLOB *pDataIn, LPCWSTR szDataDescr, DATA_BLOB *pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct, DWORD dwFlags, DATA_BLOB *pDataOut);
typedef WINBOOL (WINAPI *CryptUnprotectDataFunc) (DATA_BLOB *pDataIn, LPWSTR *ppszDataDescr, DATA_BLOB *pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct, DWORD dwFlags, DATA_BLOB *pDataOut);

static CryptProtectDataFunc pCryptProtectData;
static CryptUnprotectDataFunc pCryptUnprotectData;
static int lib_init = 0;

static void __attribute__((constructor)) init(void)
{
    static HMODULE lib;
    lib = LoadLibrary(L"crypt32.dll");
    if (lib == NULL)
        return;

    pCryptProtectData = (CryptProtectDataFunc)GetProcAddress(lib, "CryptProtectData");
    pCryptUnprotectData = (CryptUnprotectDataFunc)GetProcAddress(lib, "CryptUnprotectData");
    if (pCryptProtectData == NULL || pCryptUnprotectData == NULL) {
        FreeLibrary(lib);
        return;
    }
    lib_init = 1;
}

QString CryptData::encode(QString &txt, QString password)
{
    BOOL r;
    DATA_BLOB DataIn;
    DATA_BLOB Opt;
    DATA_BLOB DataOut;
    QByteArray res;

    if (lib_init == 0)
        return password;

    DataIn.pbData = (BYTE*)password.toUtf8().data();
    DataIn.cbData = password.toUtf8().size();

    Opt.pbData = (BYTE*)txt.toUtf8().data();
    Opt.cbData = txt.toUtf8().size();

    r = pCryptProtectData(&DataIn, NULL, &Opt, NULL, NULL, 0, &DataOut);
    if (r == false)
        return res;

    res.setRawData((const char*)DataOut.pbData, DataOut.cbData);
    return QLatin1String(res.toBase64());
}

QString CryptData::decode(QString &txt, QString _enc)
{
    BOOL r;
    DATA_BLOB DataIn;
    DATA_BLOB Opt;
    DATA_BLOB DataOut;
    QByteArray enc;
    QString res;

    if (lib_init == 0)
        return _enc;

    enc = QByteArray::fromBase64(_enc.toLatin1());

    DataIn.pbData = (BYTE*)enc.data();
    DataIn.cbData = enc.size();

    Opt.pbData = (BYTE*)txt.toAscii().data();
    Opt.cbData = txt.toAscii().size();

    r = pCryptUnprotectData(&DataIn, NULL, &Opt, NULL, NULL, 0, &DataOut);
    if (r == false)
        return res;

    res.fromUtf8((const char*)DataOut.pbData, DataOut.cbData);
    return res;
}

#else

static QString CryptData::encode(QString& str, QString &password)
{
    return password;
}

static QString CryptData::decode(QString& str, QString &_enc)
{
    return _enc;
}

#endif
