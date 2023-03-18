#ifndef SCHEMA2INDEXESMODEL_H
#define SCHEMA2INDEXESMODEL_H

#include <QAbstractTableModel>
#include "schema2status.h"
class Schema2Index;

class Schema2IndexesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Schema2IndexesModel(QObject *parent = nullptr);

    void insertIndex(const QString& name, const QStringList& columns, bool primary, Status status);

    void removeIndex(const QString& name);

    QStringList queries(const QString &tableName) const;

    void altered();

protected:
    QList<Schema2Index*> mIndexes;

    QList<Schema2Index*> mRemoveQueue;

    int find(const QString &name);
signals:

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // SCHEMA2INDEXESMODEL_H
