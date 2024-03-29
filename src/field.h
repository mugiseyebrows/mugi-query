// DO NOT EDIT ** This file was generated by mugicpp ** DO NOT EDIT
#ifndef FIELD_H
#define FIELD_H
#include <QString>

class Field {

public:
    Field();
    Field(const QString& name, const QString& type = "INT", int size = -1, bool primaryKey = false,
          bool autoincrement = false, bool index = false, bool unique = false,
          const QString& foreignKey = QString());
    QString name() const;
    void setName(const QString& value);
    QString type() const;
    void setType(const QString& value);
    int size() const;
    void setSize(int value);
    bool primaryKey() const;
    void setPrimaryKey(bool value);
    bool autoincrement() const;
    void setAutoincrement(bool value);
    bool index() const;
    void setIndex(bool value);
    bool unique() const;
    void setUnique(bool value);
    QString foreignKey() const;
    void setForeignKey(const QString& value);

protected:
    QString mName;
    QString mType;
    int mSize;
    bool mPrimaryKey;
    bool mAutoincrement;
    bool mIndex;
    bool mUnique;
    QString mForeignKey;
};

#endif // FIELD_H
