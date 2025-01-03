#ifndef SCHEMA2TABLESMODEL_H
#define SCHEMA2TABLESMODEL_H

#include <QAbstractTableModel>

class Schema2TableItem;
class Schema2TableModel;
class QGraphicsScene;
class Schema2RelationItem2;
class Schema2Relation;
class QSqlDriver;
class Schema2TreeModel;
class Schema2TreeProxyModel;
class QSqlDatabase;

#include <QPointF>
#include "hash.h"
#include "schema2status.h"
#include "uncheckedmode.h"
#include "sdata.h"

class Schema2TablesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Roles {
        SchemaRole = Qt::UserRole
    };

    explicit Schema2TablesModel(const QString &connectionName, QGraphicsScene* scene, QObject *parent = nullptr);

    Schema2TableModel* table(const SName &name);



    Schema2TableModel *tableRemoved(const SName &tableName);

    QList<Schema2TableItem*> tableItems() const;

    Schema2TableItem* tableItem(const SName &name) const;

    void setChecked(const QStringList& names, bool value);

    void setChecked(const QString& name, bool value);

    void setChecked(bool value);

    void loadPos();

    void savePos();

    void setTableItemsPos();

    QList<Schema2TableModel*> tables() const;

    SNames tableNames() const;

    QList<Schema2Relation*> relationsFrom(const SName &tableName);

    QList<Schema2Relation*> relationsTo(const SName& tableName);

    Schema2TableModel* findChildTable(Schema2Relation* relation);

    Schema2TableItem* findItem(Schema2TableModel* model);

    Schema2RelationItem2 *findItem(Schema2Relation *relation, Schema2TableItem *childTable, Schema2TableItem *parentTable);

    void dropRelation();

    QList<QPair<SName, Schema2Relation*>> dropRelationQueue() {
        return mDropRelationsQueue;
    }

    void relationDropped(const SName& name, Schema2Relation* relation) {
        mDropRelationsQueue.removeOne({name, relation});
    }

    void setUncheckedMode(UncheckedMode mode);

    //void updateColumn(const QString &tableName, const QString &name, const QString &type, bool notNull, const QString &default_, bool autoIncrement, const QString &prev);

    Schema2TreeModel* tree() const;

    Schema2TreeProxyModel* treeProxy() const;

    QList<STable> tablesState() const;

    QList<SRelation> relationsState() const;

    void merge(const STablesDiff& diff);
    void merge(const SRelationsDiff &diff);

    Schema2TableModel *tableCreated(const SName &name, Status status, const QString &engine = {});

    int indexOf(const SName &name) const;
    int indexOf(const QString &name) const;
    bool contains(const SName &name);
    bool contains(const QString &name);

    QStringList schemaNames() const;

    QSqlDatabase database() const;

protected:

    Schema2TreeModel* mTreeModel;

    Schema2TreeProxyModel* mTreeProxyModel;

    QList<QPair<SName, Schema2Relation*>> mDropRelationsQueue;

    QList<Schema2TableItem*> mTableItems;

    QList<Schema2RelationItem2*> mRelationItems;

    QList<Schema2TableModel*> mTableModels;

    QList<Schema2TableItem*> mSetPosQueue;

    QHash<SName, QPointF> mTablePos;

    QGraphicsScene* mScene;

    QString mConnectionName;

    void tableDropped(const SName &name);
    void tableRenamed(const SRenamed &table);
    void tableAltered(const STable &table);

signals:

    void tableClicked(SName, QPointF);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    // QAbstractItemModel interface
public:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    // QAbstractItemModel interface
public:
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void relationCreated(const QString &constraintName, const SName &childTable, const QStringList &childColumns,
                         const SName &parentTable, const QStringList &parentColumns, bool constrained, Status status);
    void relationRemoved(Schema2Relation *relation);


    QStringList createTablesQueries(const QSqlDatabase &db) const;
    QStringList createIndexesQueries(const QSqlDatabase &db) const;
    QStringList createRelationsQueries(const QSqlDatabase &db) const;
    //void updateColumns(const QString &tableName);
    QStringList checked(bool value) const;

    void relationCreated(const SRelation &relation, bool constrained, Status status = StatusExisting);
    void relationDropped(const SRelation &relation);
    Schema2TableModel *table(const QString &name);

    SName tableNameAt(const QModelIndex &index);
};

#endif // SCHEMA2TABLESMODEL_H
