
#ifndef SESSIONITEM_H
#define SESSIONITEM_H

#include <QList>
#include <QVariant>

class SessionItem
{
public:
    SessionItem(const QString& name, bool isSession, SessionItem *parent = 0);
    ~SessionItem();

    void appendChild(SessionItem *child);
    void removeChild(SessionItem *item);

    SessionItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    SessionItem *parent();

    QString name() const;

    bool isSession() const;

    QString prevName();
    QString nextName();

    SessionItem *lastChild();

private:
    QList<SessionItem*> childItems;
    QString mName;
    SessionItem *parentItem;
    bool mIsSession;
    int mNumber;
};

#endif
