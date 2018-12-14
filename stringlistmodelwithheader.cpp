#include "stringlistmodelwithheader.h"

StringListModelWithHeader::StringListModelWithHeader(const QStringList &strings,
                                                     QObject *parent)
    : QStringListModel(strings, parent)
{

}

QVariant StringListModelWithHeader::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (mHorizontalHeader.contains(section)) {
                return mHorizontalHeader[section];
            }
        } else {
            if (mVerticalHeader.contains(section)) {
                return mVerticalHeader[section];
            }
        }
    }
    return QStringListModel::headerData(section,orientation,role);
}

bool StringListModelWithHeader::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        if (orientation == Qt::Horizontal) {
            mHorizontalHeader[section] = value;
        } else {
            mVerticalHeader[section] = value;
        }
        emit headerDataChanged(orientation,section,section);
        return true;
    }
    return QStringListModel::setHeaderData(section,orientation,role);
}
