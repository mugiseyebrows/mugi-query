#ifndef MULTINAMEENUM_H
#define MULTINAMEENUM_H

#include <QStringList>

class MultinameEnum
{
public:
    MultinameEnum(const QStringList& ns1,
                  const QStringList& ns2 = QStringList(),
                  const QStringList& ns3 = QStringList(),
                  const QStringList& ns4 = QStringList(),
                  const QStringList& ns5 = QStringList());

    QString regExp(bool cap = true) const;

    int indexOf(const QString& value) const;

protected:
    QList<QStringList> mNames;
};

#endif // MULTINAMEENUM_H
