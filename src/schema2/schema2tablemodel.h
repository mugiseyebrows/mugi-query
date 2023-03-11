#ifndef SCHEMA2TABLEMODEL_H
#define SCHEMA2TABLEMODEL_H

#include <QAbstractItemModel>
#include "hash.h"
class Schema2Index;

class Schema2TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum cols {
        col_name,
        col_newname,
        col_type,
        col_newtype,
        cols_count
    };

    explicit Schema2TableModel(const QString& name, bool existing, QObject *parent = nullptr);

    void insertColumnsIfNotContains(const QString& name, const QString& type, const QString &prev);

    QString tableName() const;

    QString name(int row) const;

    QString newName(int row) const;

    QString type(int row) const;

    QString newType(int row) const;

    bool hasPendingChanges() const;

    QStringList newNames() const;

    Schema2Index* getIndex(const QString& name) const;

    QList<Schema2Index*> getIndexes() const {
        return mIndexes.values();
    }

    void insertIndex(const QString& name, const QStringList& columns, bool existing);

    void removeIndex(const QString& name);

    bool isIndexColumn(const QString &column) const;

signals:
    void tableClicked(QString);

protected:
    QList<QStringList> mColumns;
    QString mTableName;

    StringHash<Schema2Index*> mIndexes;

    bool mExisting;

signals:

public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};

#endif // SCHEMA2TABLEMODEL_H
