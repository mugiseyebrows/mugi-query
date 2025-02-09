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
    QStringList procedures;
};

#endif // COMPLETERDATA_H
