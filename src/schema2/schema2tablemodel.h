#ifndef SCHEMA2TABLEMODEL_H
#define SCHEMA2TABLEMODEL_H

#include <QAbstractItemModel>
#include "hash.h"
#include "schema2relation.h"
#include "schema2status.h"
class Schema2Index;
class Schema2IndexesModel;

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

#if 0
    Schema2Index* getIndex(const QString& name) const;

    QList<Schema2Index*> getIndexes() const;
#endif

    void insertIndex(const QString& name, const QStringList& columns, bool primary, Status status);

    void removeIndex(const QString& name);

    //bool isIndexColumn(const QString &column) const;

    bool containsRelation(const QString& name) const;

    Schema2Relation* insertRelation(const QString& name, const QStringList& childColumns,
                        const QString& parentTable, const QStringList& parentColumns,
                        bool constrained, Status status);

    void removeRelation(const QString& name);

    Schema2Relation* getRelation(const QString& name) const;

    StringHash<Schema2Relation*> getRelations() const;

    Schema2Relation* getRelationTo(const QString& tableName) const;

    void altered();

    Status status() const;

    QStringList createQueries() const;

    QStringList alterQueries() const;

    QStringList dropQueries() const;

    void setStatus(Status status) {
        mStatus = status;
    }

    Schema2IndexesModel* indexes() const {
        return mIndexes;
    }

signals:
    void tableClicked(QString);

protected:
    QList<QStringList> mColumns;
    QString mTableName;

    //StringHash<Schema2Index*> mIndexes;

    StringHash<Schema2Relation*> mRelations;

    Schema2IndexesModel* mIndexes;

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
