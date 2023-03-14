#ifndef SCHEMA2RELATIONSLISTMODEL2_H
#define SCHEMA2RELATIONSLISTMODEL2_H

#include <QObject>
#include <QAbstractTableModel>
class Schema2Relation;

class Schema2RelationsListModel2 : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Schema2RelationsListModel2(const QList<Schema2Relation*> relations, QObject *parent = nullptr);

signals:

protected:
    QList<Schema2Relation*> mRelations;

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // SCHEMA2RELATIONSLISTMODEL2_H
