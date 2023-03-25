#ifndef SCHEMA2TABLESMODEL_H
#define SCHEMA2TABLESMODEL_H

#include <QObject>
class Schema2TableItem;
class Schema2TableModel;
class QGraphicsScene;
#include <QPointF>
#include "hash.h"
#include "schema2status.h"

class Schema2TablesModel : public QObject
{
    Q_OBJECT
public:
    explicit Schema2TablesModel(const QString &connectionName, QGraphicsScene* scene, QObject *parent = nullptr);

    Schema2TableModel* table(const QString &name) {
        return mTableModels.get(name);
    }

    bool contains(const QString& table) {
        return mTableModels.contains(table);
    }

    void tablePulled(const QString &table, Status status);

    Schema2TableItem* tableItem(const QString& name) {
        return mTableItems.get(name);
    }

    void setGrayed(const QString& name, bool value);

    void loadPos();

    void savePos();

    void setTableItemsPos();

    QList<Schema2TableModel*> tables() const;

    QStringList tableNames() const;

protected:

    StringHash<Schema2TableItem*> mTableItems;

    StringHash<Schema2TableModel*> mTableModels;

    QList<Schema2TableItem*> mSetPosQueue;

    StringHash<QPointF> mTablePos;

    QGraphicsScene* mScene;

    QString mConnectionName;

signals:

    void tableClicked(QString);

};

#endif // SCHEMA2TABLESMODEL_H
