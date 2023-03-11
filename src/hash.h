#ifndef HASH_H
#define HASH_H

#include <QHash>
#include <QStringList>

template<typename T>
class StringHash
{
public:

    StringHash &operator=(const QHash<QString, T> &other) {
        QStringList keys = other.keys();
        for(const QString& key: keys) {
            set(key, other[key]);
        }
        return *this;
    }

    QStringList keys() const {
        QStringList items = mData.keys();
        QStringList res = {};
        for(const QString& item: items) {
            res.append(mCase[item]);
        }
        return res;
    }

    QList<T> values() const {
        return mData.values();
    }

    T get(const QString& key) const {
        return mData[key.toLower()];
    }

    void set(const QString& key, T value) {
        mData[key.toLower()] = value;
        mCase[key.toLower()] = key;
    }

    void remove(const QString& key) {
        mData.remove(key.toLower());
        mCase.remove(key.toLower());
    }

    bool contains(const QString &key) const {
        return mData.contains(key.toLower());
    }

    QHash<QString, T> mData;
    QHash<QString, QString> mCase;
};


template<typename T>
class StringListHash {
public:
    QList<QStringList> keys() const {
        QList<QStringList> items = mData.keys();
        QList<QStringList> res = {};
        for(const QStringList& item: items) {
            res.append(mCase[item]);
        }
        return res;
    }

    StringListHash &operator=(const QHash<QStringList, T> &other) {
        QList<QStringList> keys = other.keys();
        for(const QString& key: keys) {
            set(key, other[key]);
        }
        return *this;
    }

    QStringList toLower(const QStringList &items) const {
        QStringList res;
        for(const QString& item: items) {
            res.append(item.toLower());
        }
        return res;
    }

    QList<T> values() const {
        return mData.values();
    }

    T get(const QStringList& key) const {
        return mData[toLower(key)];
    }

    void set(const QStringList& key, T value) {
        mData[toLower(key)] = value;
        mCase[toLower(key)] = key;
    }

    bool contains(const QStringList &key) const {
        return mData.contains(toLower(key));
    }

    QHash<QStringList, T> mData;
    QHash<QStringList, QStringList> mCase;
};


#endif // HASH_H
