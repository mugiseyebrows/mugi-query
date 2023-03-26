#ifndef SCHEMA2RELATIONSMODEL_H
#define SCHEMA2RELATIONSMODEL_H

#include <QAbstractTableModel>

#include "hash.h"
class Schema2Relation;
#include "schema2status.h"

class Schema2RelationsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Schema2RelationsModel(QObject *parent = nullptr);

    Schema2Relation *insert(const QString &name, const QStringList &childColumns, const QString &parentTable,
                                    const QStringList &parentColumns, bool constrained, Status status);
    Schema2Relation *get(const QString &name) const;

    bool contains(const QString &name) const;

    Schema2Relation* remove(const QString &name);

    Schema2Relation *remove(Schema2Relation *relation);

    Schema2Relation *getRelationTo(const QString &tableName) const;

    QList<Schema2Relation*> values() const;

    Schema2Relation * at(int index) const;

protected:
    QList<Schema2Relation*> mRelations;

    int indexOf(const QString &name) const ;
signals:


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    Schema2Relation *removeAt(int index);
};

#endif // SCHEMA2RELATIONSMODEL_H
