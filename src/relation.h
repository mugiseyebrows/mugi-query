#ifndef RELATION_H
#define RELATION_H

#include <QStringList>

class Relation {
public:
    Relation();
    Relation(const QString& primary,const QString& foreign);
    bool isEmpty() const;
    QString primaryTable() const;
    QString foreignTable() const;
    QString primaryKey() const;
    QString foreignKey() const;
    void setPrimary(int table);
    void setForeign(int table);
    int primary() const;
    int foreign() const;
protected:
    QString mPrimaryKey;
    QString mForeignKey;
    QString mPrimaryTable;
    QString mForeignTable;
    int mPrimary;
    int mForeign;
};

#endif // RELATION_H
