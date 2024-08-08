#include "dataimportmodel.h"

#include "sqldatatypes.h"
#include <QColor>
#include <QDebug>

DataImportModel::DataImportModel(QObject *parent)
    : QStandardItemModel(5,5,parent)
{

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

            QMetaType::Type type = mTypes[index.column()];
            int size = mSizes[index.column()];
            if (type == QMetaType::QString) {
                if (size > -1) {
                    return v.toString().size() > size ? QVariant(QColor(Qt::red)) : QVariant();
                }
                return QVariant();
            }

            int minYear = 1950;
            SqlDataTypes::tryConvert(v, type, mLocale, minYear, true, false, &ok);

            if (!ok) {
                return QVariant(QColor(Qt::red));
            }
        } else {
            return QVariant(QColor(Qt::red));
        }
    }
    return QStandardItemModel::data(index,role);
}

void DataImportModel::setTypes(const QMap<int, QMetaType::Type> &types, const QMap<int, int> &sizes)
{
    if (mTypes == types && mSizes == sizes) {
        //qDebug() << "same types";
        return;
    }
    mTypes = types;
    mSizes = sizes;
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
}

void DataImportModel::setLocale(const QLocale &locale)
{
    mLocale = locale;
}
