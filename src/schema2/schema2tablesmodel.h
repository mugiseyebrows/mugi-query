#ifndef SCHEMA2TABLESMODEL_H
#define SCHEMA2TABLESMODEL_H

#include <QAbstractTableModel>
class Schema2TableItem;
class Schema2TableModel;
class QGraphicsScene;
class Schema2RelationItem2;
class Schema2Relation;
class QSqlDriver;

#include <QPointF>
#include "hash.h"
#include "schema2status.h"
#include "uncheckedmode.h"

class Schema2TablesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Schema2TablesModel(const QString &connectionName, QGraphicsScene* scene, QObject *parent = nullptr);

    Schema2TableModel* table(const QString &name);

    bool contains(const QString& table);

    Schema2TableModel *tablePulled(const QString &table, Status status);

    Schema2TableModel *tableRemoved(const QString &tableName);

    QList<Schema2TableItem*> tableItems() const;

    Schema2TableItem* tableItem(const QString& name) const;

    void setGrayed(const QString& name, bool value);

    void setAllGrayed(bool value);

    void loadPos();

    void savePos();

    void setTableItemsPos();

    QList<Schema2TableModel*> tables() const;

    QStringList tableNames() const;

    QList<Schema2Relation*> relationsFrom(const QString& tableName);

    QList<Schema2Relation*> relationsTo(const QString& tableName);

    Schema2TableModel* findChildTable(Schema2Relation* relation);

    Schema2TableItem* findItem(Schema2TableModel* model);

    Schema2RelationItem2 *findItem(Schema2Relation *relation, Schema2TableItem *childTable, Schema2TableItem *parentTable);

    void dropRelation();

    QList<QPair<QString, Schema2Relation*>> dropRelationQueue() {
        return mDropRelationsQueue;
    }

    void relationDropped(QString name, Schema2Relation* relation) {
        mDropRelationsQueue.removeOne({name, relation});
    }

    void setUncheckedMode(UncheckedMode mode);


protected:

    QList<QPair<QString, Schema2Relation*>> mDropRelationsQueue;

    QList<Schema2TableItem*> mTableItems;

    QList<Schema2RelationItem2*> mRelationItems;

    QList<Schema2TableModel*> mTableModels;

    QList<Schema2TableItem*> mSetPosQueue;

    StringHash<QPointF> mTablePos;

    QGraphicsScene* mScene;

    QString mConnectionName;

    int indexOf(const QString &name) const;
signals:

    void tableClicked(QString);

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
    void relationPulled(const QString &constraintName, const QString &childTable, const QStringList &childColumns, const QString &parentTable, const QStringList &parentColumns, bool constrained, Status status);
    void relationRemoved(Schema2Relation *relation);


    QStringList createTablesQueries(const QString &driverName, QSqlDriver *driver) const;
    QStringList createIndexesQueries(const QString &driverName, QSqlDriver *driver) const;
    QStringList createRelationsQueries(const QString &driverName, QSqlDriver *driver) const;
};

#endif // SCHEMA2TABLESMODEL_H
