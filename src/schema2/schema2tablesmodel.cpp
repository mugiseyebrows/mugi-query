#include "schema2tablesmodel.h"

#include <QGraphicsScene>
#include "schema2tableitem.h"
#include <QDebug>
#include "schema2store.h"
#include "schema2tablemodel.h"
#include "schema2tableitem.h"


Schema2TablesModel::Schema2TablesModel(const QString& connectionName, QGraphicsScene *scene, QObject *parent)
    : mConnectionName(connectionName), mScene(scene), QAbstractTableModel{parent}
{

}

int Schema2TablesModel::indexOf(const QString& name) {

    for(int i=0;i<mTableModels.size();i++) {
        if (mTableModels[i]->tableName().toLower() == name.toLower()) {
            return i;
        }
    }
    return -1;
}

Schema2TableModel *Schema2TablesModel::table(const QString &name) {
    int index = indexOf(name);
    if (index < 0) {
        return 0;
    }
    return mTableModels[index];
}

bool Schema2TablesModel::contains(const QString &table) {
    return indexOf(table) > -1;
}

void Schema2TablesModel::tablePulled(const QString& table, Status status) {

    if (contains(table)) {
        return;
    }
    int row = rowCount();

    Schema2TableModel* model = new Schema2TableModel(table, status);

    connect(model, SIGNAL(tableClicked(QString)), this, SIGNAL(tableClicked(QString)));
    Schema2TableItem* item = new Schema2TableItem(model);
    if (mTablePos.contains(table)) {
        item->setPos(mTablePos.get(table));
    } else {
        mSetPosQueue.append(item);
    }

    beginInsertRows(QModelIndex(), row, row);
    mTableModels.append(model);
    mTableItems.append(item);
    endInsertRows();
    mScene->addItem(item);
}

Schema2TableItem *Schema2TablesModel::tableItem(const QString &name) {
    int index = indexOf(name);
    if (index < 0) {
        return 0;
    }
    return mTableItems[index];
}

void Schema2TablesModel::setGrayed(const QString &name, bool value) {
    auto* table = tableItem(name);
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
    for(Schema2TableItem* item: qAsConst(mTableItems)) {
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
    return mTableModels;
}

QStringList Schema2TablesModel::tableNames() const
{
    QStringList res;
    for(Schema2TableModel* model: qAsConst(mTableModels)) {
        res.append(model->tableName());
    }
    return res;
}

int Schema2TablesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mTableModels.size();
}

int Schema2TablesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return 1;
}

QVariant Schema2TablesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::EditRole || role == Qt::DisplayRole) {
        if (index.column() == 0) {
            return mTableModels[index.row()]->tableName();
        }
    }
    if (role == Qt::CheckStateRole) {
        if (index.column() == 0) {
            return mTableItems[index.row()]->grayed() ? Qt::Unchecked : Qt::Checked;
        }
    }
    return QVariant();
}


bool Schema2TablesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    if (role == Qt::CheckStateRole) {
        bool checked = value.toInt() == Qt::Checked;
        mTableItems[index.row()]->setGrayed(!checked);
        emit dataChanged(index, index);
    }
    return false;
}


Qt::ItemFlags Schema2TablesModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
}
