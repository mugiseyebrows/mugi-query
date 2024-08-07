#ifndef COMPLETERDATA_H
#define COMPLETERDATA_H

#include <QStringList>

class CompleterData
{
public:
    QStringList fields;
    QStringList tables;
    QStringList keywords;
    QStringList functions;
    QStringList types;
};

#endif // COMPLETERDATA_H
