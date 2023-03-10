#ifndef SCHEMA2RELATIONSLISTMODEL_H
#define SCHEMA2RELATIONSLISTMODEL_H

#include <QObject>
#include <QStandardItemModel>
#include "hash.h"

class Schema2RelationModel;

class Schema2RelationsListModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit Schema2RelationsListModel(const StringListHash<Schema2RelationModel*>& relationModels,
                                       QObject *parent = nullptr);

signals:

};

#endif // SCHEMA2RELATIONSLISTMODEL_H
