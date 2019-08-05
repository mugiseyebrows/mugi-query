#include "multinameenum.h"

#include <QSet>

MultinameEnum::MultinameEnum(const QStringList &ns1, const QStringList &ns2, const QStringList &ns3, const QStringList &ns4, const QStringList &ns5)
{
    if (!ns1.isEmpty()) {
        mNames.append(ns1);
    }
    if (!ns2.isEmpty()) {
        mNames.append(ns2);
    }
    if (!ns3.isEmpty()) {
        mNames.append(ns3);
    }
    if (!ns4.isEmpty()) {
        mNames.append(ns4);
    }
    if (!ns5.isEmpty()) {
        mNames.append(ns5);
    }
}

namespace {

QString group(const QString& s, bool cap = true) {
    return (cap ? "(" : "(?:") + s + ")";
}

QString group(const QStringList& s, bool cap = true) {
    return (cap ? "(" : "(?:") + s.join("|") + ")";
}

QStringList column(const QList<QStringList>& data, int i) {
    QStringList result;
    foreach(const QStringList& item, data) {
        result << item.value(i);
    }
    return result;
}

}


QString MultinameEnum::regExp() const
{
    int size = mNames[0].size();
    QStringList result;
    for(int i=0;i<size;i++) {
        result << group(column(mNames, i).toSet().toList(),false);
    }
    return group(result);
}

int MultinameEnum::indexOf(const QString &value) const
{
    foreach(const QStringList& item, mNames) {
        int index = item.indexOf(value);
        if (index > -1) {
            return index;
        }
    }
    return -1;
}
