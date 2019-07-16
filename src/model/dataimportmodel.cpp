#include "dataimportmodel.h"

DataImportModel::DataImportModel(const QVariant& headerSizeHint, QObject *parent)
    : QStandardItemModel(5,5,parent), mHeaderSizeHint(headerSizeHint)
{

}

QVariant DataImportModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::SizeHintRole && orientation == Qt::Horizontal) {
        return mHeaderSizeHint;
    }
    return QStandardItemModel::headerData(section,orientation,role);
}
