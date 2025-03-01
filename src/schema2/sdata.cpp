#include "sdata.h"
#include <QSet>

template <typename T>
static QSet<T> toSet(const QList<T>& qlist)
{
    return QSet<T> (qlist.constBegin(), qlist.constEnd());
}

template <typename T>
static QList<T> toList(const QSet<T>& qlist)
{
    return QList<T> (qlist.constBegin(), qlist.constEnd());
}

int countSame(const QSet<QString>& names1, const QSet<QString>& names2) {
    return (names1 & names2).size();
}

int countDifferent(const QSet<QString>& names1, const QSet<QString>& names2) {
    return (names1 - names2).size() + (names2 - names1).size();
}

class SScore {
public:
    SScore(int same, int diff) : same(same), diff(diff) {

    }
    int same;
    int diff;
};

int findSimilar(const STable& table, const QList<STable>& tables) {
    QList<SScore> scores;
    auto columnNames = toSet(table.columnNames());
    for(int i=0;i<tables.size();i++) {
        const STable& table1 = tables[i];
        auto columnNames1 = toSet(table1.columnNames());
        scores.append(SScore(countSame(columnNames, columnNames1),
                             countDifferent(columnNames, columnNames1)));
    }
    int sameLimit = columnNames.size() < 4 ? columnNames.size() : columnNames.size() / 2;
    int index = -1;
    for(int i=0;i<tables.size();i++) {
        if (scores[i].same >= sameLimit) {
            if (index < 0) {
                index = i;
            } else {
                if (scores[i].same > scores[index].same) {
                    index = i;
                } else if (scores[i].same == scores[index].same) {
                    if (scores[i].diff < scores[index].diff) {
                        index = i;
                    }
                }
            }
        }
    }
    return index;
}

static SNames getSNames(const QList<STable>& tables) {
    SNames res;
    for(const STable& table: tables) {
        res.append(table.name);
    }
    return res;
}

QStringList getNames(const QList<STable> &tables)
{
    QStringList res;
    for(const STable& table: tables) {
        res.append(table.fullname());
    }
    return res;
}

STablesDiff getDiff(const QList<STable> &tables1, const QList<STable> &tables2) {

    QSet<QString> names1 = toSet(getNames(tables1));
    QSet<QString> names2 = toSet(getNames(tables2));

    QList<STable> created = withName(tables2, toList(names2 - names1));
    QList<STable> dropped = withName(tables1, toList(names1 - names2));

    QList<SRenamed> renamed;

    QStringList names = getNames(created);
    for(const QString& name: names) {
        STable table = withName(created, name);
        int index = findSimilar(table, dropped);
        if (index > -1) {
            STable before = dropped.takeAt(index);
            STable after = table;
            created.removeAt(created.indexOf(table));
            renamed.append(SRenamed(after, before.name, after.name));
        }
    }

    QList<STable> altered;
    names = toList(names1 & names2);
    for(const QString& name: names) {
        STable table1 = withName(tables1, name);
        STable table2 = withName(tables2, name);
        if (table1 != table2) {
            altered.append(table2);
        }
    }

    return STablesDiff(created, getSNames(dropped), altered, renamed);
}


QList<STable> withName(const QList<STable> &tables, const QStringList &names)
{
    QList<STable> res;
    QStringList names1 = getNames(tables);
    for(int i=0;i<names1.size();i++) {
        if (names.contains(names1[i])) {
            res.append(tables[i]);
        }
    }
    return res;
}

STable withName(const QList<STable> &tables, const QString &name)
{
    for(int i=0;i<tables.size();i++) {
        if (tables[i].fullname() == name) {
            return tables[i];
        }
    }
    return STable();
}

QStringList STable::columnNames() const
{
    QStringList res;
    for(const SColumn& column: columns) {
        res.append(column.name);
    }
    return res;
}

bool operator ==(const STable &table1, const STable &table2) {
    return table1.type == table2.type
            && table1.name == table2.name
           && table1.columns == table2.columns
        && table1.engine == table2.engine;
}

bool operator !=(const STable &table1, const STable &table2) {
    return !(table1 == table2);
}

bool operator ==(const SColumn &column1, const SColumn &column2) {
    return column1.name == column2.name
           && column1.type == column2.type
           && column1.notNull == column2.notNull
           && column1.default_ == column2.default_
           && column1.autoIncrement == column2.autoIncrement;
}

bool operator !=(const SColumn &column1, const SColumn &column2) {
    return !(column1 == column2);
}

bool operator == (const QList<SColumn>& column1, const QList<SColumn>& column2) {
    if (column1.size() != column2.size()) {
        return false;
    }
    for(int i=0;i<column1.size();i++) {
        if (column1[i] != column2[i]) {
            return false;
        }
    }
    return true;
}

bool operator != (const QList<SColumn>& column1, const QList<SColumn>& column2) {
    return !(column1 == column2);
}

bool operator ==(const SRelation &relation1, const SRelation &relation2) {
    return relation1.name == relation2.name
           && relation1.childTable == relation2.childTable
           && relation1.childColumns == relation2.childColumns
           && relation1.parentTable == relation2.parentTable
           && relation1.parentColumns == relation2.parentColumns;
}

bool operator !=(const SRelation &relation1, const SRelation &relation2) {
    return !(relation1 == relation2);
}

bool operator == (const SName& name1, const SName& name2) {
    return (name1.schema == name2.schema && name1.name == name2.name)
           || (name1.schema.toLower() == name2.schema.toLower() && name1.name.toLower() == name2.name.toLower());
}

uint qHash(const SName &key) {
    return qHash(key.fullname().toLower());
}

bool operator != (const SName& name1, const SName& name2) {
    return !(name1 == name2);
}

QDebug operator << (QDebug dbg, const SName& name) {
    dbg << name.fullname();
    return dbg;
}

SRelationsDiff getDiff(const QList<SRelation> &relations1, const QList<SRelation> &relations2)
{
    QList<SRelation> dropped;
    QList<SRelation> created;

    for(const auto& relation: relations1) {
        if (relations2.indexOf(relation) < 0) {
            dropped.append(relation);
        }
    }
    for(const auto& relation: relations2) {
        if (relations1.indexOf(relation) < 0) {
            created.append(relation);
        }
    }

    //qDebug() << "dropped" << dropped.size() << dropped;
    //qDebug() << "created" << created.size() << created;

    return SRelationsDiff(dropped, created);
}

QDebug operator <<(QDebug &dbg, const SRelation &relation) {
    dbg.nospace() << QString("SRelation(%1 -> %2)").arg(relation.childTable.name).arg(relation.parentTable.name);
    return dbg;
}

int indexOf(const SNames &tables, const QString &table) {

    QString table_ = table.toLower();
    if (table.contains(".")) {
        for(int i=0;i<tables.size();i++) {
            if (tables[i].fullname().toLower() == table_) {
                return i;
            }
        }
    } else {
        for(int i=0;i<tables.size();i++) {
            if (tables[i].name.toLower() == table_) {
                return i;
            }
        }
    }
    return -1;
}

static SNames getNames(const QList<SStored>& stored) {
    SNames res;
    for(const SStored& item: stored) {
        res.append(item.name);
    }
    return res;
}

SStoredDiff getDiff(const QList<SStored> &stored1, const QList<SStored> &stored2) {

    SNames names1 = getNames(stored1);
    SNames names2 = getNames(stored2);

    QList<SStored> created;
    SNames removed;

    for(const SName& name: names1.names) {
        if (names2.indexOf(name) < 0) {
            removed.append(name);
        }
    }
    for(const SName& name: names2.names) {
        if (names1.indexOf(name) < 0)  {
            int index = names2.indexOf(name);
            created.append(stored2[index]);
        }
    }

    SStoredDiff diff;
    diff.created = created;
    diff.removed = removed;
    return diff;
}

int indexOf(const QList<SStored> &stored, const SName &name) {
    for(int i=0;i<stored.size();i++) {
        const SStored& item = stored[i];
        if (item.name == name) {
            return i;
        }
    }
    return -1;
}

QString SRelation::joinCondition(bool parentChild) const {
    QStringList res;
    QString pattern = parentChild ? "%1.%2=%3.%4" : "%3.%4=%1.%2";
    for(int i=0;i<parentColumns.size();i++) {
        res.append(pattern.arg(parentTable.name).arg(parentColumns[i]).arg(childTable.name).arg(childColumns[i]));
    }
    return res.join(" and ");
}
