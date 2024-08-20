#ifndef SCHEMA2TABLEMODEL_H
#define SCHEMA2TABLEMODEL_H

#include <QAbstractItemModel>
#include "hash.h"
#include "schema2relation.h"
#include "schema2status.h"
#include "schema2tablecolumn.h"
#include "sdata.h"

class Schema2Index;
class Schema2IndexesModel;
class Schema2RelationsModel;
class QSqlDriver;
class Schema2ParentRelationsModel;
class Schema2TablesModel;


class Schema2TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum cols {
        col_name,
        col_type,
        col_notnull,
        col_default,
        col_autoincrement,

        col_name_prev,
        col_type_prev,
        col_notnull_prev,
        col_default_prev,
        col_autoincrement_prev,
        cols_count
    };

    explicit Schema2TableModel(const QString& name, Status status, QObject *parent = nullptr);

    bool updateColumn(const SColumn &column, const QString &prev);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

    QString tableName() const;

    void setTableName(const QString& name);

    QString namePrev(int row) const;

    QString name(int row) const;

    QString typePrev(int row) const;

    QString type(int row) const;

    bool notNull(int row) const;

    bool notNullPrev(int row) const;

    bool autoincrement(int row) const;

    bool autoincrementPrev(int row) const;

    QString defaultPrev(int row) const;

    QString default_(int row) const;

    bool hasPendingChanges() const;

    QStringList columnNames() const;

    void setName(int row, const QString& value);

    void setType(int row, const QString& value);

#if 0
    Schema2Index* getIndex(const QString& name) const;

    QList<Schema2Index*> getIndexes() const;
#endif

    void insertIndex(const QString& name, const QStringList& columns, bool primary, bool unique, Status status);

    void removeIndex(const QString& name);

    //bool isIndexColumn(const QString &column) const;

    bool containsRelation(const QString& name) const;

    Schema2Relation* insertRelation(const QString& name, const QStringList& childColumns,
                        const QString& parentTable, const QStringList& parentColumns,
                        bool constrained, Status status);

    Schema2Relation* removeRelation(const QString& name);

    Schema2Relation* removeRelation(Schema2Relation* relation);

    Schema2Relation* relation(const QString& name) const;

    Schema2RelationsModel* relations() const;

    QList<Schema2Relation*> relationsTo(const QString& tableName) const;

    void pushed(bool created);

    Status status() const;

    QStringList createQueries(const QString &driverName, QSqlDriver *driver) const;

    QStringList alterQueries(const QString &driverName, QSqlDriver *driver) const;

    QStringList dropQueries(const QString &driverName, QSqlDriver *driver) const;

    QStringList autoincrementQueries(const QString &driverName, QSqlDriver *driver) const;

    void setStatus(Status status);

    Schema2IndexesModel* indexes() const;

    bool contains(Schema2Relation* relation);

    QStringList relatedTables() const;

    QStringList relationNames() const;

    QStringList relationsChildColumns() const;

    QStringList indexNames() const;

    Schema2ParentRelationsModel *parentRelations() const;

    QStringList names() const;

    SColumn at(int row) const;

signals:
    void tableClicked(QString, QPointF);


protected:
    //QList<QStringList> mColumns;

    QList<SColumn> mColumns;
    QList<SColumn> mColumnsPrev;


    QString mTableName;

    //StringHash<Schema2Index*> mIndexes;

    //StringHash<Schema2Relation*> mRelations;

    QList<QString> mDropColumnsQueue;

    Schema2RelationsModel* mRelations;

    Schema2ParentRelationsModel* mParentRelations;

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

    QString columnDefinition(const QString &driverName, QSqlDriver *driver, int row, bool skipAutoIncrement, const QStringList &primaryKey) const;

    // QAbstractItemModel interface
public:
    bool removeRows(int row, int count, const QModelIndex &parent);
    void updateParentRelations(Schema2TablesModel *tables);

    int indexOf(const QString &name);
    void tableAltered(const STable &table);

};

#endif // SCHEMA2TABLEMODEL_H
