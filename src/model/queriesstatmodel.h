#ifndef QUERIESSTATMODEL_H
#define QUERIESSTATMODEL_H

#include <QObject>
#include <QStandardItemModel>

class QueriesStatModel : public QStandardItemModel
{
public:
    QueriesStatModel(const QStringList& queries,
                     const QStringList errors,
                     const QList<int> &perf,
                     const QList<int> &rowsAffected,
                     QObject *parent = 0);

    bool hasErrors() const;

protected:
    bool mHasErrors;
};

#endif // QUERIESSTATMODEL_H
