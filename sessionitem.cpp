
#include <QStringList>
#include "sessionitem.h"

SessionItem::SessionItem(const QString &name, bool isSession, SessionItem *parent)
    : mName(name), parentItem(parent), mIsSession(isSession), mNumber(0)
{

}

SessionItem::~SessionItem()
{
    if (parentItem) {
        parentItem->removeChild(this);
    }
    qDeleteAll(childItems);
}

void SessionItem::appendChild(SessionItem *item)
{
    childItems.append(item);
}

void SessionItem::removeChild(SessionItem *item) {
    childItems.removeOne(item);
}

SessionItem *SessionItem::child(int row)
{
    return childItems.value(row);
}

SessionItem* SessionItem::lastChild() {
    return childItems.isEmpty() ? 0 : childItems[childItems.size()-1];
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

QString SessionItem::prevName() {
    if (!childItems.isEmpty()) {
        return lastChild()->name();
    }
    for(int row = this->row() - 1; row >= 0; row++) {
        SessionItem* child = parentItem->child(row)->lastChild();
        if (child) {
            return child->name();
        }
    }
    return QString();
}

QString SessionItem::nextName()
{
    return QString("%1 %2").arg(mName).arg(++mNumber);
}

int SessionItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<SessionItem*>(this));
    return 0;
}
