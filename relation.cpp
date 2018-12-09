#include "relation.h"

Relation::Relation()
{

}

Relation::Relation(const QString &primary, const QString &foreign) {
    QStringList s = primary.split(".");
    mPrimaryTable = s.value(0);
    mPrimaryKey = s.value(1);
    s = foreign.split(".");
    mForeignTable = s.value(0);
    mForeignKey = s.value(1);
}

bool Relation::isEmpty() const {
    return mPrimaryTable.isEmpty() || mForeignTable.isEmpty();
}

QString Relation::primaryTable() const {
    return mPrimaryTable;
}

QString Relation::foreignTable() const {
    return mForeignTable;
}

QString Relation::primaryKey() const {
    return mPrimaryKey;
}

QString Relation::foreignKey() const {
    return mForeignKey;
}

void Relation::setPrimary(int table) {
    mPrimary = table;
}

void Relation::setForeign(int table) {
    mForeign = table;
}

int Relation::primary() const {
    return mPrimary;
}

int Relation::foreign() const {
    return mForeign;
}
