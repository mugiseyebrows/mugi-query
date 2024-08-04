#ifndef SDATA_H
#define SDATA_H

#include <QString>
#include <QStringList>

class SColumn {
public:
    SColumn() {

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

bool operator == (const STable& table1, const STable& table2);

bool operator != (const STable& table1, const STable& table2);

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

class SDiff {
public:
    SDiff() {

    }
    SDiff(const QList<STable>& created, const QStringList& dropped, const QList<STable>& altered,
          const QList<SRenamed>& renamed)
        : created(created), dropped(dropped), altered(altered), renamed(renamed) {

    }

    QList<STable> created;
    QStringList dropped;
    QList<STable> altered;
    QList<SRenamed> renamed;
};

QStringList getNames(const QList<STable>& tables);

QList<STable> withName(const QList<STable>& tables, const QStringList& names);

STable withName(const QList<STable>& tables, const QString& name);

SDiff getDiff(const QList<STable>& tables1, const QList<STable>& tables2);

#endif // SDATA_H
