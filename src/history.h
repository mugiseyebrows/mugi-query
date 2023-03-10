#ifndef HISTORY_H
#define HISTORY_H

#include <QString>
#include <QObject>

class History
{
public:

    static History* instance();

    void addQuery(const QString &database, const QString &query);
    void addDatabase(const QString &connectionName, const QString &driver, const QString &host, const QString &user, const QString &password, const QString &database, int port);

protected:
    static History* mInstance;
    History(QObject* parent = 0);

};

#endif // HISTORY_H
