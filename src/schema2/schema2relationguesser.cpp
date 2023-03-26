#include "schema2relationguesser.h"

#include "schema2tablemodel.h"
#include "schema2indexesmodel.h"
#include "tolower.h"

Schema2RelationGuesser::Schema2RelationGuesser(Schema2TableModel* childTable,
                                               Schema2TableModel* parentTable,
                                               QObject *parent)
    : mParentTable(parentTable), mChildTable(childTable), QObject{parent}
{

}

QString Schema2RelationGuesser::relationName()
{
    return QString("fk_%1_%2").arg(mParentTable->tableName()).arg(mChildTable->tableName());
}

QStringList Schema2RelationGuesser::parentColumns()
{   
    QStringList primaryKey = mParentTable->indexes()->primaryKey();
    if (!primaryKey.isEmpty()) {
        return primaryKey;
    }
    QString tableName = mParentTable->tableName().toLower();
    QStringList columnNames = toLower(mParentTable->newNames());
    if (columnNames.isEmpty()) {
        return {};
    }
    QStringList guesses = {"id", "objid", QString("%1_id").arg(tableName), QString("%1_id").arg(tableName)};
    for(const QString& guess: guesses) {
        if (columnNames.contains(guess)) {
            return {guess};
        }
    }
    return {columnNames[0]};
}

bool containsAll(const QStringList& haystack, const QStringList& needle) {
    QStringList haystack1 = toLower(haystack);
    QStringList needle1 = toLower(needle);
    for(const QString& item: needle1) {
        if (!haystack1.contains(item)) {
            return false;
        }
    }
    return true;
}

QStringList Schema2RelationGuesser::childColumns(const QStringList &parentColumns)
{
    QStringList columns = toLower(mChildTable->newNames());

    if (containsAll(columns, parentColumns)) {
        return parentColumns;
    }
    if (parentColumns.size() == 1) {
        QString parentTable = mParentTable->tableName().toLower();
        for(const QString& column: columns) {
            if (column.contains(parentTable)) {
                return {column};
            }
        }
    }
    return columns.mid(0, parentColumns.size());
}
