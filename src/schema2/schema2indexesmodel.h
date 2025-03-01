#ifndef SCHEMA2INDEXESMODEL_H
#define SCHEMA2INDEXESMODEL_H

#include <QAbstractTableModel>
#include "schema2status.h"
class Schema2Index;
class QSqlDriver;
class Schema2TableModel;
class QSqlDatabase;

class Schema2IndexesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum cols {
        col_name,
        col_columns,
        col_primary,
        col_unique,
        cols_count
    };

    explicit Schema2IndexesModel(QObject *parent = nullptr);

    void insertIndex(const QString& name, const QStringList& columns, bool primary, bool unique, Status status);

    void removeIndex(const QString& name);

    void removeAt(int index);

    Schema2Index* get(const QString& name) const;

    QStringList queries(Schema2TableModel* table, const QSqlDatabase& db) const;

    void pushed();

    QStringList primaryKey() const;

    QList<Schema2Index*> values() const {
        return mIndexes;
    }

    //QStringList foreignKeys() const;

protected:
    QList<Schema2Index*> mIndexes;

    QList<Schema2Index*> mRemoveQueue;

    int find(const QString &name) const;
signals:

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void debugStatus();

    // QAbstractItemModel interface
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void primaryKeysCreated();
};

#endif // SCHEMA2INDEXESMODEL_H
