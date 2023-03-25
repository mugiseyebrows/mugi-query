#include "schema2tablesmodel.h"

#include <QGraphicsScene>
#include "schema2tableitem.h"
#include <QDebug>
#include "schema2store.h"
#include "schema2tablemodel.h"
#include "schema2tableitem.h"

Schema2TablesModel::Schema2TablesModel(const QString& connectionName, QGraphicsScene *scene, QObject *parent)
    : mConnectionName(connectionName), mScene(scene), QObject{parent}
{

}

void Schema2TablesModel::tablePulled(const QString& table, Status status) {
    if (!mTableModels.contains(table)) {
        Schema2TableModel* model = new Schema2TableModel(table, status);
        mTableModels.set(table, model);
        connect(model, SIGNAL(tableClicked(QString)), this, SIGNAL(tableClicked(QString)));
    }
    Schema2TableModel* model = mTableModels.get(table);
    if (!mTableItems.contains(table)) {
        Schema2TableItem* item = new Schema2TableItem(model);
        if (mTablePos.contains(table)) {
            item->setPos(mTablePos.get(table));
        } else {
            mSetPosQueue.append(item);
        }
        mTableItems.set(table, item);
        mScene->addItem(item);
    }
}

void Schema2TablesModel::setGrayed(const QString &name, bool value) {
    auto* table = mTableItems.get(name);
    if (!table) {
        qDebug() << "!table" << __FILE__ << __LINE__;
        return;
    }
    table->setGrayed(value);
}

void Schema2TablesModel::loadPos() {
    mTablePos = Schema2Store::instance(this)->loadPos(mConnectionName);
}

void Schema2TablesModel::savePos()
{
    QHash<QString, QPointF> pos;
    QList<Schema2TableItem*> items = mTableItems.values();
    for(Schema2TableItem* item: qAsConst(items)) {
        pos[item->tableName()] = item->pos();
    }
    Schema2Store::instance(this)->savePos(mConnectionName, pos);
}

void Schema2TablesModel::setTableItemsPos()
{
    int w = 200;
    int s = 25;
    int spacing = 40;
    int h = 900;

    int x = 0;
    int y = 0;

    for(Schema2TableItem* item: mSetPosQueue) {
        int height = item->boundingRect().height();
        if ((y + height + spacing) > h) {
            x += w + spacing;
            y = 0;
            item->setPos(x, y);
        } else {
            item->setPos(x, y);
        }
        y += height + spacing;
    }
}

QList<Schema2TableModel *> Schema2TablesModel::tables() const
{
    return mTableModels.values();
}

QStringList Schema2TablesModel::tableNames() const
{
    return mTableModels.keys();
}
