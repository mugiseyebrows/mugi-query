#ifndef IMPORTDATACOLUMNMODEL_H
#define IMPORTDATACOLUMNMODEL_H

#include "model/checkablestringlistmodel.h"

class DataImportColumnModel : public CheckableStringListModel
{
    Q_OBJECT
public:
    DataImportColumnModel(const QStringList &names, const QList<QMetaType::Type>& types, QObject *parent = 0);

    void checkState(Qt::CheckState state, QStringList &names, QList<QMetaType::Type> &types) const;
    void checked(QStringList &names, QList<QMetaType::Type> &types) const;
    QList<bool> checkList() const;
protected:
    QList<QMetaType::Type> mTypes;
};

#endif // IMPORTDATACOLUMNMODEL_H
