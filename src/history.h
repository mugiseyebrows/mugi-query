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

    void addJoin(const QString &connectionName1, const QString &query1, const QStringList &columns1,
                 const QString &connectionName2, const QString &query2, const QStringList &columns2);
protected:
    static History* mInstance;
    History(QObject* parent = 0);

};

#endif // HISTORY_H
