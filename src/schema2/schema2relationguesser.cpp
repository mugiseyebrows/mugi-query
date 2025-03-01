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

static bool isAllUpper(const QString& text) {
    for(int i=0;i<text.size();i++) {
        if (text[i].isLetter()) {
            if (!text[i].isUpper()) {
                return false;
            }
        }
    }
    return true;
}

static bool isAllLower(const QString& text) {
    for(int i=0;i<text.size();i++) {
        if (text[i].isLetter()) {
            if (!text[i].isLower()) {
                return false;
            }
        }
    }
    return true;
}

QString Schema2RelationGuesser::relationName()
{
    //return QString("fk_%1_%2").arg(mParentTable->tableName()).arg(mChildTable->tableName());

    auto childColumns = this->childColumns(this->parentColumns());

    QString suffix = "fkey";
    if (isAllUpper(mChildTable->tableName().name) && isAllUpper(childColumns[0])) {
        suffix = "FKEY";
    }

    return QString("%1_%2_%3")
            .arg(mChildTable->tableName().name)
            .arg(childColumns.join("_"))
            .arg(suffix);
}

QStringList Schema2RelationGuesser::parentColumns()
{   
    QStringList primaryKey = mParentTable->indexes()->primaryKey();
    if (!primaryKey.isEmpty()) {
        return primaryKey;
    }
    QString tableName = mParentTable->tableName().name.toLower();
    QStringList columnNames = toLower(mParentTable->columnNames());
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
    QStringList columns = toLower(mChildTable->columnNames());

    if (containsAll(columns, parentColumns)) {
        return parentColumns;
    }
    if (parentColumns.size() == 1) {
        QString parentTable = mParentTable->tableName().name.toLower();
        for(const QString& column: columns) {
            if (column.contains(parentTable)) {
                return {column};
            }
        }
    }
    return columns.mid(0, parentColumns.size());
}
