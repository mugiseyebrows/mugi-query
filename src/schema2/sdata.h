#ifndef SDATA_H
#define SDATA_H

#include <QString>
#include <QStringList>
#include <QDebug>

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


class STable {
public:
    STable() {

    }
    STable(const QString& name, const QList<SColumn>& columns) : name(name), columns(columns) {

    }

    QString name;
    QList<SColumn> columns;
    QStringList columnNames() const;
};

class SRelation {
public:
    SRelation() {

    }
    SRelation(QString name, QString childTable,
              QStringList childColumns,
              QString parentTable, QStringList parentColumns) :
        name(name), childTable(childTable), childColumns(childColumns),
        parentTable(parentTable), parentColumns(parentColumns)
    {

    }

    QString name;
    QString childTable;
    QStringList childColumns;
    QString parentTable;
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
    SRenamed(const STable& table, const QString oldName, const QString& newName)
        : table(table), oldName(oldName), newName(newName) {

    }

    STable table;
    QString oldName;
    QString newName;
};

class STablesDiff {
public:
    STablesDiff() {

    }
    STablesDiff(const QList<STable>& created, const QStringList& dropped, const QList<STable>& altered,
          const QList<SRenamed>& renamed)
        : created(created), dropped(dropped), altered(altered), renamed(renamed) {

    }

    bool isEmpty() const {
        return created.isEmpty() && dropped.isEmpty() && altered.isEmpty() && renamed.isEmpty();
    }

    QList<STable> created;
    QStringList dropped;
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
