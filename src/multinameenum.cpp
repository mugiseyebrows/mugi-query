#include "multinameenum.h"

#include <QSet>
#include <algorithm>

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

QString group(const QStringList& s, bool cap = true) {
    return (cap ? "(" : "(?:") + s.join("|") + ")";
}

}

QString MultinameEnum::regExp(bool cap) const
{
    QStringList result;
    foreach(const QStringList& names, mNames) {
        result.append(names);
    }
    // sort by length in descending order
    std::sort(result.begin(),result.end(),[](const QString& s1, const QString& s2){
        return s1.size() > s2.size();
    });
    return group(result, cap);
}

int MultinameEnum::indexOf(const QString &value) const
{
    foreach(const QStringList& names, mNames) {
        int index = names.indexOf(value);
        if (index > -1) {
            return index;
        }
    }
    return -1;
}
