#include "dataimportmodel.h"

#include "sqldatatypes.h"
#include <QColor>

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

QVariant DataImportModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ForegroundRole) {
        if (mTypes.contains(index.column())) {
            bool ok;
            QVariant v = data(index);
            if (v.isNull()) {
                return QVariant();
            }
            SqlDataTypes::tryConvert(v,mTypes[index.column()],mLocale,&ok);
            if (!ok) {
                return QVariant(QColor(Qt::red));
            }
        }
    }
    return QStandardItemModel::data(index,role);
}

void DataImportModel::setTypes(const QMap<int, QVariant::Type> &types)
{
    mTypes = types;
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
}

void DataImportModel::setLocale(const QLocale &locale)
{
    mLocale = locale;
}
