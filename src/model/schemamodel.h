#ifndef SCHEMAMODEL_H
#define SCHEMAMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QMap>
#include "schemaitem.h"
#include "tokens.h"

class SchemaModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    SchemaModel(QObject* parent = 0);
    ~SchemaModel() override;

    void update(const QMap<QString,Tokens>& tokens);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    QModelIndex find(const QString& connectionName);

    QModelIndex parent(const QModelIndex &child) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

protected:

    void setChildren(const QModelIndex &index, const QStringList &data);

    SchemaItem *getItem(const QModelIndex &parent) const;

    SchemaItem* mRoot;

};

#endif // SCHEMAMODEL_H
