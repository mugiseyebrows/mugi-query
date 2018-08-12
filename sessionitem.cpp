
#include <QStringList>
#include "sessionitem.h"

SessionItem::SessionItem(const QString &name, bool isSession, SessionItem *parent)
    : mName(name), parentItem(parent), mIsSession(isSession)
{

}

SessionItem::~SessionItem()
{
    qDeleteAll(childItems);
}

void SessionItem::appendChild(SessionItem *item)
{
    childItems.append(item);
}

SessionItem *SessionItem::child(int row)
{
    return childItems.value(row);
}

int SessionItem::childCount() const
{
    return childItems.count();
}

int SessionItem::columnCount() const
{
    return 1;
}

QVariant SessionItem::data(int column) const
{
    if (column == 0) {
        return mName;
    }
    return QVariant();
}

SessionItem *SessionItem::parent()
{
    return parentItem;
}

QString SessionItem::name() const
{
    return mName;
}

bool SessionItem::isSession() const
{
    return mIsSession;
}

int SessionItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<SessionItem*>(this));
    return 0;
}
