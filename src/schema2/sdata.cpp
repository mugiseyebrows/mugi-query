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

SDiff getDiff(const QList<STable> &tables1, const QList<STable> &tables2) {

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

    return SDiff(created, getNames(dropped), altered, renamed);
}

QStringList getNames(const QList<STable> &tables)
{
    QStringList res;
    for(const STable& table: tables) {
        res.append(table.name);
    }
    return res;
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
        if (tables[i].name == name) {
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
    return table1.name == table2.name && table1.columns == table2.columns;
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
