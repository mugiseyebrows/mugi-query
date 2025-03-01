
#include <QtGui>
#include "sessionitem.h"
#include "sessionmodel.h"

SessionModel::SessionModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new SessionItem(QString(),false);
}

SessionModel::~SessionModel()
{
    delete rootItem;
}

int SessionModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<SessionItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

bool SessionModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    if (parent.isValid()) {
        // add session

        SessionItem* parentItem = static_cast<SessionItem*>(parent.internalPointer());
        QString name = parentItem->nextName();
        SessionItem* child = new SessionItem(name,true,parentItem);
        QString prevName = parentItem->prevName();
        parentItem->appendChild(child);
        emit sessionAdded(parentItem->name(),child->name(),prevName);

    } else {
        // add database

        SessionItem* child = new SessionItem(mName,false,rootItem);
        rootItem->appendChild(child);
    }
    endInsertRows();
    return true;
}

bool SessionModel::removeRows(int row, int count, const QModelIndex &parent)
{

    QList<SessionItem*> sessions;
    QStringList names;

    for(int i=0;i<count;i++) {
        SessionItem* session = static_cast<SessionItem*>(index(row + i,0,parent).internalPointer());
        sessions << session;
        names << session->name();
    }

    if (sessions[0]->isSession()) {
        // sessions
        QString database = sessions[0]->parent()->name();
        foreach(const QString& name, names) {
            emit sessionRemoved(database,name);
        }
    }

    beginRemoveRows(parent, row, row + count - 1);
    qDeleteAll(sessions);
    endRemoveRows();
    return true;
}

void SessionModel::addDatabase(const QString &name)
{
    mName = name;
    insertRows(rowCount(),1);
}

void SessionModel::addSession(const QModelIndex &parent)
{

    QModelIndex index = parent;

    SessionItem* item = static_cast<SessionItem*>(index.internalPointer());

    if (item->isSession()) {
        index = index.parent();
    }

    int row = this->rowCount(index);
    insertRow(row,index);
#if 0
    QString db = data(parent).toString();


    QString name = data(index(row,0,parent)).toString();
    QString namePrev;

    if (row == 0) {
        if (parent.row()>0) {
            QModelIndex parent_ = index(parent.row()-1,0);
            namePrev = data(index(rowCount(parent_)-1,0,parent_)).toString();
        }
    } else {
        namePrev = data(index(row-1,0,parent)).toString();
    }

    emit sessionAdded(db,name,namePrev);
#endif
}

void SessionModel::removeDatabase(const QModelIndex &index)
{
    int rowCount = this->rowCount(index);
    if (rowCount > 0) {
        removeRows(0,rowCount,index);
    }
    removeRow(index.row());
}

void SessionModel::removeSession(const QModelIndex &index)
{
    //SessionItem* item = static_cast<SessionItem*>(index.internalPointer());
    //emit sessionRemoved(item->parent()->name(),item->name());
    removeRow(index.row(),index.parent());
}

bool SessionModel::isSession(const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    SessionItem* item = static_cast<SessionItem*>(index.internalPointer());
    return item->isSession();
}

bool SessionModel::isDatabase(const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }
    SessionItem* item = static_cast<SessionItem*>(index.internalPointer());
    return !item->isSession();
}

QString SessionModel::connectionName(const QModelIndex &index)
{
    if (!index.isValid()) {
        return QString();
    }
    SessionItem* item = static_cast<SessionItem*>(index.internalPointer());
    if (item->isSession()) {
        return item->parent()->name();
    }
    return item->name();
}

QString SessionModel::sessionName(const QModelIndex &index)
{
    if (!index.isValid()) {
        return QString();
    }
    SessionItem* item = static_cast<SessionItem*>(index.internalPointer());
    if (!item->isSession()) {
        return QString();
    }
    return item->name();
}

QModelIndex SessionModel::indexOf(const QString &connectionName, const QString &name) const
{
    for(int i=0;i<rootItem->childCount();i++) {
        if (rootItem->child(i)->name() == connectionName) {
            SessionItem* database_ = rootItem->child(i);
            for(int j=0;j<database_->childCount();j++) {
                if (database_->child(j)->name() == name) {
                    return this->index(j,0,this->index(i,0));
                }
            }
        }
    }
    return QModelIndex();
}

QModelIndex SessionModel::indexOf(const QString &connectionName) const {
    for(int i=0;i<rootItem->childCount();i++) {
        if (rootItem->child(i)->name() == connectionName) {
            return index(i,0);
        }
    }
    return QModelIndex();
}

QStringList SessionModel::connectionNames() const
{
    QStringList res;
    for(int row=0;row<rootItem->childCount();row++) {
        QString name = rootItem->child(row)->name();
        res.append(name);
    }
    return res;
}

QVariant SessionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();
    SessionItem *item = static_cast<SessionItem*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags SessionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant SessionModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == 0) {
            return "Sessions";
        }
    }
    return QVariant();
}

QModelIndex SessionModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();
    SessionItem *parentItem;
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SessionItem*>(parent.internalPointer());
    SessionItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex SessionModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
    SessionItem *childItem = static_cast<SessionItem*>(index.internalPointer());
    SessionItem *parentItem = childItem->parent();
    if (parentItem == rootItem)
        return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
}

int SessionModel::rowCount(const QModelIndex &parent) const
{
    SessionItem *parentItem;
    if (parent.column() > 0)
        return 0;
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SessionItem*>(parent.internalPointer());
    return parentItem->childCount();
}

