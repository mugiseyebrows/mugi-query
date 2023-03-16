#ifndef SCHEMA2TABLEMODEL_H
#define SCHEMA2TABLEMODEL_H

#include <QAbstractItemModel>
#include "hash.h"
#include "schema2relation.h"
#include "schema2status.h"
class Schema2Index;

class Schema2TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum cols {
        col_name,
        col_newname,
        col_type,
        col_newtype,
        cols_count
    };

    explicit Schema2TableModel(const QString& name, Status status, QObject *parent = nullptr);

    void insertColumnsIfNotContains(const QString& name, const QString& type, const QString &prev);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    QString tableName() const;

    QString name(int row) const;

    QString newName(int row) const;

    QString type(int row) const;

    QString newType(int row) const;

    bool hasPendingChanges() const;

    QStringList newNames() const;

    Schema2Index* getIndex(const QString& name) const;

    QList<Schema2Index*> getIndexes() const {
        return mIndexes.values();
    }

    void insertIndex(const QString& name, const QStringList& columns, bool existing);

    void removeIndex(const QString& name);

    bool isIndexColumn(const QString &column) const;

    bool containsRelation(const QString& name) const;

    Schema2Relation* insertRelation(const QString& name, const QStringList& childColumns,
                        const QString& parentTable, const QStringList& parentColumns,
                        bool constrained, Status status);

    void removeRelation(const QString& name);

    Schema2Relation* getRelation(const QString& name) const;

    StringHash<Schema2Relation*> getRelations() const;

    Schema2Relation* getRelationTo(const QString& tableName) const;

    void altered() {
        mStatus = StatusExisting;
        for(int row=0;row<rowCount();row++) {
            QString newName = this->newName(row);
            QString newType = this->newType(row);
            setData(index(row, col_name), newName);
            setData(index(row, col_type), newType);
        }
    }

    Status status() const {
        if (mStatus == StatusNew) {
            return StatusNew;
        }
        if (mStatus == StatusExisting) {
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

    QStringList createQueries() const {
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

    QStringList alterQueries() const {

        QStringList res;

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

    QStringList dropQueries() const {
        QString expr = QString("DROP TABLE %1").arg(mTableName);
        return {expr};
    }

    void setStatus(Status status) {
        mStatus = status;
    }

signals:
    void tableClicked(QString);

protected:
    QList<QStringList> mColumns;
    QString mTableName;

    StringHash<Schema2Index*> mIndexes;

    StringHash<Schema2Relation*> mRelations;

    Status mStatus;

signals:

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};

#endif // SCHEMA2TABLEMODEL_H
