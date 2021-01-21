#ifndef IMPORTDATACOLUMNMODEL_H
#define IMPORTDATACOLUMNMODEL_H

#include "model/checkablestringlistmodel.h"

class DataImportColumnModel : public CheckableStringListModel
{
    Q_OBJECT
public:
    DataImportColumnModel(const QStringList &names, const QList<QVariant::Type>& types, QObject *parent = 0);

    void checkState(Qt::CheckState state, QStringList &names, QList<QVariant::Type> &types) const;
    void checked(QStringList &names, QList<QVariant::Type> &types) const;
    QList<bool> checkList() const;
protected:
    QList<QVariant::Type> mTypes;
};

#endif // IMPORTDATACOLUMNMODEL_H
