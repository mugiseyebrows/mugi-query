#ifndef SCHEMA2STOREDMODEL_H
#define SCHEMA2STOREDMODEL_H

#include <QAbstractTableModel>
#include "sdata.h"

class Schema2StoredModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Schema2StoredModel(const QString& connectionName, QObject *parent = nullptr);

signals:

    // QAbstractItemModel interface
public:
    enum Columns {
        ColumnSchema,
        ColumnName,
        ColumnType,
        ColumnCount
    };

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QList<SStored> state() const;

    void merge(const SStoredDiff& diff);

protected:

    QList<SStored> mData;
    QString mConnectionName;
};

#endif // SCHEMA2STOREDMODEL_H
