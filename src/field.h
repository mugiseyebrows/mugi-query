#ifndef FIELD_H
#define FIELD_H
#include <QString>

class Field {
public:
    Field();
    Field(const QString& name, const QString& type, int size, bool primaryKey, bool autoincrement,
          bool index, bool unique);
    QString name() const;
    void setName(const QString& name);
    QString type() const;
    void setType(const QString& type);
    int size() const;
    void setSize(int size);
    bool primaryKey() const;
    void setPrimaryKey(bool primaryKey);
    bool autoincrement() const;
    void setAutoincrement(bool autoincrement);
    bool index() const;
    void setIndex(bool index);
    bool unique() const;
    void setUnique(bool unique);

protected:
    QString mName;
    QString mType;
    int mSize;
    bool mPrimaryKey;
    bool mAutoincrement;
    bool mIndex;
    bool mUnique;
};
#endif // FIELD_H
