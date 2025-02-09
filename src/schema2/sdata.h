#ifndef SDATA_H
#define SDATA_H

#include <QString>
#include <QStringList>
#include <QDebug>
#include "enums.h"

class SColumn {
public:
    SColumn() : notNull(false), autoIncrement(false) {

    }
    SColumn(const QString& name, const QString& type, bool notNull = false, const QString& default_ = QString(), bool autoIncrement = false)
        : name(name), type(type), notNull(notNull), default_(default_), autoIncrement(autoIncrement) {

    }

    QString name;
    QString type;
    bool notNull;
    QString default_;
    bool autoIncrement;
};

bool operator == (const SColumn& column1, const SColumn& column2);
bool operator != (const SColumn& column1, const SColumn& column2);

bool operator == (const QList<SColumn>& column1, const QList<SColumn>& column2);

bool operator != (const QList<SColumn>& column1, const QList<SColumn>& column2);

class SName {
public:
    SName() {

    }

    SName(const QString& name) : name(name) {

    }

    SName(const QString& schema, const QString& name) : schema(schema), name(name) {

    }

    QString fullname() const {
        if (schema.isEmpty()) {
            return name;
        }
        return schema + "." + name;
    }

    QString schema;
    QString name;
};

bool operator == (const SName& name1, const SName& name2);
bool operator != (const SName& name1, const SName& name2);

QDebug operator << (QDebug dbg, const SName& name);

uint qHash(const SName &key);

class SNames {
public:
    SNames() {

    }

    SNames(std::initializer_list<SName> args) : names(args) {

    }

    bool isEmpty() const {
        return names.isEmpty();
    }

    int size() const {
        return names.size();
    }

    void append(const SName& name) {
        names.append(name);
    }

    const SName& operator [](int index) const {
        return names.at(index);
    }

    int indexOf(const SName& name) const {
        for(int i=0;i<names.size();i++) {
            if (name == names[i]) {
                return i;
            }
        }
        return -1;
    }

    bool contains(const SName& name) const {
        for(const SName& n: std::as_const(names)) {
            if (n == name) {
                return true;
            }
        }
        return false;
    }

    QStringList getNames() const {
        QStringList res;
        for(const auto& name: std::as_const(names)) {
            res.append(name.name);
        }
        return res;
    }

    void removeAt(int index) {
        names.removeAt(index);
    }

    QString join(const QString& sep) const {
        return getNames().join(sep);
    }

    QList<SName> names;
};

int findTable(const SNames& tables, const QString& table);

class STable {
public:

    STable() {

    }

    STable(const QString& name, const QList<SColumn>& columns)
        : type(TableType::Table), name(name), columns(columns) {

    }

    STable(const SName& name, const QList<SColumn>& columns, const QString& engine = {}, TableType type = TableType::Table)
        : type(type), name(name), columns(columns), engine(engine) {

    }

    QString fullname() const {
        return name.fullname();
    }

    TableType type;
    SName name;
    QList<SColumn> columns;
    QString engine;
    QStringList columnNames() const;
};

class SRelation {
public:
    SRelation() {

    }
    SRelation(QString name, const SName& childTable,
              QStringList childColumns,
              const SName& parentTable, QStringList parentColumns) :
        name(name), childTable(childTable), childColumns(childColumns),
        parentTable(parentTable), parentColumns(parentColumns)
    {

    }

    QString name;
    SName childTable;
    QStringList childColumns;
    SName parentTable;
    QStringList parentColumns;
};

bool operator == (const STable& table1, const STable& table2);

bool operator != (const STable& table1, const STable& table2);

bool operator == (const SRelation& relation1, const SRelation& relation2);

bool operator != (const SRelation& relation1, const SRelation& relation2);

class SRenamed {
public:
    SRenamed() {

    }
    SRenamed(const STable& table, const SName& oldName, const SName& newName)
        : table(table), oldName(oldName), newName(newName) {

    }

    STable table;
    SName oldName;
    SName newName;
};

class STablesDiff {
public:
    STablesDiff() {

    }
    STablesDiff(const QList<STable>& created, const SNames& dropped, const QList<STable>& altered,
          const QList<SRenamed>& renamed)
        : created(created), dropped(dropped), altered(altered), renamed(renamed) {

    }

    bool isEmpty() const {
        return created.isEmpty() && dropped.isEmpty() && altered.isEmpty() && renamed.isEmpty();
    }

    QList<STable> created;
    SNames dropped;
    QList<STable> altered;
    QList<SRenamed> renamed;
};

QStringList getNames(const QList<STable>& tables);

QList<STable> withName(const QList<STable>& tables, const QStringList& names);

STable withName(const QList<STable>& tables, const QString& name);

STablesDiff getDiff(const QList<STable>& tables1, const QList<STable>& tables2);

class SRelationsDiff {
public:
    SRelationsDiff(const QList<SRelation>& dropped, const QList<SRelation>& created) : dropped(dropped), created(created) {

    }
    QList<SRelation> dropped;
    QList<SRelation> created;
};

SRelationsDiff getDiff(const QList<SRelation>& relations1, const QList<SRelation>& relations2);

QDebug operator << (QDebug& dbg, const SRelation& relation);

#endif // SDATA_H
