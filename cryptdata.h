#ifndef CRYPTDATA_H
#define CRYPTDATA_H

#include <common.h>
#include <QString>
#include <QByteArray>

class CryptData
{
public:
    static QString encode(QString &txt, QString password);
    static QString decode(QString &txt, QString encoded);
private:

};

#endif // CRYPTDATA_H
