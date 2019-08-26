#ifndef HISTORYMODEL_H
#define HISTORYMODEL_H

#include <QObject>
#include <QSqlQueryModel>

class QueryHistoryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    enum cols {
        col_date,
        col_connectionName,
        col_query
    };
    QueryHistoryModel(QObject *parent = nullptr);
};

#endif // HISTORYMODEL_H
