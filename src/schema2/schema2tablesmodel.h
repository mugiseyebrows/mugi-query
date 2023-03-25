#ifndef SCHEMA2TABLESMODEL_H
#define SCHEMA2TABLESMODEL_H

#include <QAbstractTableModel>
class Schema2TableItem;
class Schema2TableModel;
class QGraphicsScene;
#include <QPointF>
#include "hash.h"
#include "schema2status.h"


class Schema2TablesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Schema2TablesModel(const QString &connectionName, QGraphicsScene* scene, QObject *parent = nullptr);

    Schema2TableModel* table(const QString &name);

    bool contains(const QString& table);

    void tablePulled(const QString &table, Status status);

    Schema2TableItem* tableItem(const QString& name);

    void setGrayed(const QString& name, bool value);

    void loadPos();

    void savePos();

    void setTableItemsPos();

    QList<Schema2TableModel*> tables() const;

    QStringList tableNames() const;

protected:

    QList<Schema2TableItem*> mTableItems;

    QList<Schema2TableModel*> mTableModels;

    QList<Schema2TableItem*> mSetPosQueue;

    StringHash<QPointF> mTablePos;

    QGraphicsScene* mScene;

    QString mConnectionName;

    int indexOf(const QString &name);
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
};

#endif // SCHEMA2TABLESMODEL_H
