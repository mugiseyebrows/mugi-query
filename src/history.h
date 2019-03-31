#ifndef HISTORY_H
#define HISTORY_H

#include <QString>
#include <QObject>

class History : public QObject
{
    Q_OBJECT
public:
    History(QObject* parent = 0);
    void addQuery(const QString &database, const QString &query);
    void addDatabase(const QString &connectionName, const QString &driver, const QString &host, const QString &user, const QString &password, const QString &database, int port);
};

#endif // HISTORY_H
