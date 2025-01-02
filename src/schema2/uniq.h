#ifndef UNIQ_H
#define UNIQ_H

#include <QList>

template <typename T>
QList<T> uniq(const QList<T>& values) {
    QList<T> res;
    for(const T& value: values) {
        if (!res.contains(value)) {
            res.append(value);
        }
    }
    return res;
}


#endif // UNIQ_H
