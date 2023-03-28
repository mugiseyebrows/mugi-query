#include "schema2tablemodel.h"

#include "schema2index.h"
#include "schema2relation.h"
#include "schema2indexesmodel.h"
#include "schema2relationsmodel.h"

// todo table renames

Schema2TableModel::Schema2TableModel(const QString &name, Status status, QObject *parent)
    : mTableName(name), mStatus(status), mRelations(new Schema2RelationsModel(this)), mIndexes(new Schema2IndexesModel(this)), QAbstractTableModel{parent}
{

}

void Schema2TableModel::insertColumnsIfNotContains(const QString &name, const QString &type, const QString& prev)
{
    int insertRow = rowCount();

    for(int row=0;row<mColumns.size();row++) {
        if (this->name(row) == name) {
            return;
        }
        if (this->name(row) == prev) {
            insertRow = row + 1;
        }
    }

    beginInsertRows(QModelIndex(), insertRow, insertRow);
    mColumns.insert(insertRow, {name, name, type, type});
    endInsertRows();
}

bool Schema2TableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for(int i=0;i<count;i++) {
        mColumns.insert(row, {QString(), QString(), QString(), QString()});
    }
    endInsertRows();
    return true;
}

QString Schema2TableModel::tableName() const
{
    return mTableName;
}

QString Schema2TableModel::name(int row) const
{
    return mColumns[row][col_name];
}

QString Schema2TableModel::newName(int row) const
{
    return mColumns[row][col_newname];
}

QString Schema2TableModel::type(int row) const
{
    return mColumns[row][col_type];
}

QString Schema2TableModel::newType(int row) const
{
    return mColumns[row][col_newtype];
}

bool Schema2TableModel::hasPendingChanges() const
{
    return false;
}

QStringList Schema2TableModel::newNames() const
{
    QStringList res;
    for(int row=0;row<mColumns.size();row++) {
        res.append(mColumns[row][col_newname]);
    }
    return res;
}

void Schema2TableModel::setNewName(int row, const QString &value)
{
    mColumns[row][col_newname] = value;
}

void Schema2TableModel::setNewType(int row, const QString &value)
{
    mColumns[row][col_newtype] = value;
}

#if 0
Schema2Index *Schema2TableModel::getIndex(const QString &name) const
{
    return mIndexes.get(name);
}

QList<Schema2Index *> Schema2TableModel::getIndexes() const {
    return mIndexes.values();
}
#endif

void Schema2TableModel::insertIndex(const QString &name, const QStringList &columns, bool primary, bool unique, Status status)
{
    //mIndexes.set(name, new Schema2Index(name, columns, primary, status));

    mIndexes->insertIndex(name, columns, primary, unique, status);

}

void Schema2TableModel::removeIndex(const QString &name)
{
    //mIndexes.remove(name);
    mIndexes->removeIndex(name);
}

#if 0
bool Schema2TableModel::isIndexColumn(const QString &column) const
{
    QList<Schema2Index*> indexes = mIndexes.values();
    for(Schema2Index* index: indexes) {
        if (index->isIndexColumn(column)) {
            return true;
        }
    }
    return false;
}
#endif

bool Schema2TableModel::containsRelation(const QString &name) const
{
    return mRelations->contains(name);
}

Schema2Relation* Schema2TableModel::insertRelation(const QString &name, const QStringList &childColumns,
                                                   const QString &parentTable, const QStringList &parentColumns,
                                                   bool constrained, Status status)
{
    return mRelations->insert(name, childColumns, parentTable, parentColumns, constrained, status);
}

Schema2Relation* Schema2TableModel::removeRelation(const QString &name)
{
    return mRelations->remove(name);
}

Schema2Relation *Schema2TableModel::removeRelation(Schema2Relation *relation)
{
    return mRelations->remove(relation);
}

Schema2Relation *Schema2TableModel::relation(const QString &name) const
{
    return mRelations->get(name);
}

Schema2RelationsModel *Schema2TableModel::relations() const
{
    return mRelations;
}

Schema2Relation *Schema2TableModel::relationTo(const QString &tableName) const
{
    return mRelations->getRelationTo(tableName);
}

void Schema2TableModel::pushed() {
    mStatus = StatusExisting;
    for(int row=0;row<rowCount();row++) {
        QString newName = this->newName(row);
        QString newType = this->newType(row);
        setData(index(row, col_name), newName);
        setData(index(row, col_type), newType);
    }
    mDropColumnsQueue.clear();
}

Status Schema2TableModel::status() const {
    if (mStatus == StatusNew) {
        return StatusNew;
    }
    if (mStatus == StatusExisting) {
        if (!mDropColumnsQueue.isEmpty()) {
            return StatusModified;
        }
        for(int row=0;row<rowCount();row++) {
            if (newName(row).isEmpty()) {
                continue;
            }
            if (name(row) != newName(row) || type(row) != newType(row)) {
                return StatusModified;
            }
        }
        return StatusExisting;
    }
    return mStatus;
}

QStringList Schema2TableModel::createQueries() const {
    QStringList fields;
    for(int row=0;row<rowCount();row++) {
        QString newName = this->newName(row);
        QString newType = this->newType(row);
        if (newName.isEmpty() || newType.isEmpty()) {
            continue;
        }
        fields.append(QString("%1 %2").arg(newName).arg(newType));
    }
    QString expr = QString("CREATE TABLE %1 (%2)").arg(mTableName).arg(fields.join(", "));
    return {expr};
}

QStringList Schema2TableModel::alterQueries() const {

    QStringList res;

    for(const QString& colName: mDropColumnsQueue) {
        // todo check for removed and readded
        QString expr = QString("ALTER TABLE %1 DROP COLUMN %2")
                .arg(mTableName)
                .arg(colName);
        res.append(expr);
    }

    for(int row=0;row<rowCount();row++) {

        QString name = this->name(row);
        QString newName = this->newName(row);
        QString type = this->type(row);
        QString newType = this->newType(row);

        if (newName.isEmpty()) {

        } else {
            if (name.isEmpty()) {
                QString expr = QString("ALTER TABLE %1 ADD COLUMN %2 %3")
                        .arg(mTableName)
                        .arg(newName)
                        .arg(newType);
                res.append(expr);
            } else if (name == newName) {

                if (type != newType) {
                    QString expr = QString("ALTER TABLE %1 ALTER COLUMN %2 %3")
                            .arg(mTableName)
                            .arg(newName)
                            .arg(newType);
                    res.append(expr);
                }

            } else if (name != newName) {
                // todo implement column renaming for supported drivers
            }
        }
    }

    return res;
}

QStringList Schema2TableModel::dropQueries() const {
    QString expr = QString("DROP TABLE %1").arg(mTableName);
    return {expr};
}

void Schema2TableModel::setStatus(Status status) {
    mStatus = status;
}

Schema2IndexesModel *Schema2TableModel::indexes() const {
    return mIndexes;
}

bool Schema2TableModel::contains(Schema2Relation *relation)
{
    return mRelations->values().contains(relation);
}

int Schema2TableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mColumns.size();
}

int Schema2TableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return cols_count;
}

QVariant Schema2TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return mColumns[index.row()][index.column()];
    }
    return QVariant();
}

Qt::ItemFlags Schema2TableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == col_newname || index.column() == col_newtype) {
        flags |= Qt::ItemIsEditable;
    }
    return flags;
}

bool Schema2TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        mColumns[index.row()][index.column()] = value.toString();
        emit dataChanged(index, index);
    }
    return false;
}

QVariant Schema2TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static QHash<int, QString> header = {
        {col_name, "Name"},
        {col_newname, "Name"},
        {col_type, "Type"},
        {col_newtype, "Type"},
    };

    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return header.value(section);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

bool Schema2TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), row, row+count-1);
    for(int i=0;i<count;i++) {
        QString colName = mColumns[row][col_name];
        if (!colName.isEmpty()) {
            mDropColumnsQueue.append(colName);
        }
        mColumns.removeAt(row);
    }
    endRemoveRows();
}
