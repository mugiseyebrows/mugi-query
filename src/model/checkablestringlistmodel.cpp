#include "checkablestringlistmodel.h"

#include <QDebug>

CheckableStringListModel::CheckableStringListModel(const QStringList &strings, QObject *parent)
    : QStringListModel(strings,parent)
{

}

QVariant CheckableStringListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::CheckStateRole) {
        if (index.column() == 0) {
            return mChecked.contains(index.row()) ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QStringListModel::data(index,role);
}

bool CheckableStringListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole) {
        if (index.column() == 0) {
            if (value.toInt() == Qt::Checked) {
                mChecked.insert(index.row());
            } else if (value.toInt() == Qt::Unchecked) {
                mChecked.remove(index.row());
            }
            emit dataChanged(index,index);
        }
    }
    return QStringListModel::setData(index,value,role);
}

void CheckableStringListModel::setChecked(const QStringList &itemsToCheck, bool checked) {
    Q_ASSERT(checked); // todo implement unchecking
    QStringList items = stringList();
    QSet<int> prev = mChecked;
    mChecked.clear();
    for(int row=0;row<items.size();row++) {
        if (itemsToCheck.contains(items[row])) {
            mChecked.insert(row);
        }
    }
    if (mChecked != prev) {
        emit dataChanged(index(0,0), index(rowCount()-1,0));
    }
}

void CheckableStringListModel::setChecked(const QString &item, bool checked)
{
    int index = stringList().indexOf(item);
    setData(this->index(index, 0), checked ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
}

void CheckableStringListModel::setList(const QStringList &items)
{
    QStringList checked = this->checked();
    mChecked.clear();
    setStringList(items);
    setChecked(checked);
}

Qt::ItemFlags CheckableStringListModel::flags(const QModelIndex &index) const
{
    if (index.column() == 0) {
        return QStringListModel::flags(index) | Qt::ItemIsUserCheckable;
    }
    return QStringListModel::flags(index);
}

QStringList CheckableStringListModel::checked() const
{
    return checkState(Qt::Checked);
}

QStringList CheckableStringListModel::unchecked() const
{
    return checkState(Qt::Unchecked);
}

QStringList CheckableStringListModel::checkState(Qt::CheckState state) const
{
    QStringList res;
    for(int r=0;r<rowCount();r++) {
        if (data(index(r,0),Qt::CheckStateRole).toInt() == state) {
            res << data(index(r,0)).toString();
        }
    }
    return res;
}

bool CheckableStringListModel::hasAnyChecked() const
{
    return hasAnyCheckState(Qt::Checked);
}

bool CheckableStringListModel::hasAnyUnchecked() const
{
    return hasAnyCheckState(Qt::Unchecked);
}

bool CheckableStringListModel::hasAnyCheckState(Qt::CheckState state) const
{
    for(int r=0;r<rowCount();r++) {
        if (data(index(r,0),Qt::CheckStateRole).toInt() == state) {
            return true;
        }
    }
    return false;
}

void CheckableStringListModel::setAllChecked()
{
    setAllCheckState(Qt::Checked);
}

void CheckableStringListModel::setAllUnchecked()
{
    setAllCheckState(Qt::Unchecked);
}

void CheckableStringListModel::setAllCheckState(Qt::CheckState state)
{
    QSet<int> checked = mChecked;
    if (state == Qt::Checked) {
        for(int r=0;r<rowCount();r++) {
            mChecked << r;
        }
    } else if (state == Qt::Unchecked) {
        mChecked.clear();
    } else {
        qDebug() << "state" << state << __FILE__ << __LINE__;
    }
    if (checked != mChecked) {
        emit dataChanged(index(0,0),index(rowCount()-1,1));
    }
}

QList<bool> CheckableStringListModel::checkedAsBoolList() const
{
    QList<bool> res;
    for(int r=0;r<rowCount();r++) {
        res << (data(index(r,0),Qt::CheckStateRole).toInt() == Qt::Checked);
    }
    return res;
}

int CheckableStringListModel::countChecked() const
{
    return count(Qt::Checked);
}

int CheckableStringListModel::countUnchecked() const
{
    return count(Qt::Unchecked);
}

void CheckableStringListModel::toggleChecked(const QString &item)
{
    int row = stringList().indexOf(item);
    QModelIndex index = this->index(row, 0);
    int value = data(index, Qt::CheckStateRole).toInt();
    setData(index, value == Qt::Checked ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);
}

template <class T1, class T2>
QList<QPair<T1, T2>> zip(const QList<T1>& values1, const QList<T2>& values2) {
    QList<QPair<T1, T2>> res;
    Q_ASSERT(values1.size() == values2.size());
    for(int i=0;i<values1.size();i++) {
        res.append({values1[i], values2[i]});
    }
    return res;
}

QList<QPair<QString, bool> > CheckableStringListModel::dataAsTupleList() const
{
    return zip(stringList(), checkedAsBoolList());
}

int CheckableStringListModel::count(Qt::CheckState state) const
{
#if 0
    int count = 0;
    for(int r=0;r<rowCount();r++) {
        if (data(index(r,0),Qt::CheckStateRole).toInt() == state) {
            count++;
        }
    }
    return count;
#endif
    return state == Qt::Checked ? mChecked.size() : rowCount() - mChecked.size();
}
