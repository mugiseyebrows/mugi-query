#ifndef SCHEMA2RELATIONSMODEL_H
#define SCHEMA2RELATIONSMODEL_H

#include <QAbstractTableModel>

#include "hash.h"
class Schema2Relation;
class Schema2TableModel;
#include "schema2status.h"

class Schema2RelationsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum cols {
        col_name,
        col_child_table,
        col_child_columns,
        col_parent_table,
        col_parent_columns,
        col_constrained,
        cols_count
    };

    explicit Schema2RelationsModel(Schema2TableModel* table, QObject *parent = nullptr);

    Schema2Relation *insert(const QString &name, const QStringList &childColumns, const QString &parentTable,
                                    const QStringList &parentColumns, bool constrained, Status status);
    Schema2Relation *relation(const QString &name) const;

    bool contains(const QString &name) const;

    Schema2Relation* remove(const QString &name);

    Schema2Relation *remove(Schema2Relation *relation);

    Schema2Relation *getRelationTo(const QString &tableName) const;

    QList<Schema2Relation*> values() const;

    Schema2Relation * at(int index) const;

protected:

    Schema2TableModel* mTable;

    QList<Schema2Relation*> mRelations;

    int indexOf(const QString &name) const ;
signals:


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    Schema2Relation *removeAt(int index);

    // QAbstractItemModel interface
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // SCHEMA2RELATIONSMODEL_H
