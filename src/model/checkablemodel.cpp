#include "checkablemodel.h"

CheckableModel::CheckableModel(QObject *parent, const QStringList &items)
    : QStandardItemModel{parent}
{
    append(items);
}

void CheckableModel::append(const QStringList &items)
{
    for(const QString& item: std::as_const(items)) {
        QStandardItem* standardItem = new QStandardItem(item);
        standardItem->setCheckable(true);
        appendRow(standardItem);
    }
}

void CheckableModel::setChecked(Qt::CheckState state) {
    for(int row=0;row<this->rowCount();row++) {
        auto index = this->index(row, 0);
        this->setData(index, state, Qt::CheckStateRole);
    }
}

QStringList CheckableModel::checked() const {
    QStringList res;
    for(int row=0;row<this->rowCount();row++) {
        auto index = this->index(row, 0);
        if (this->data(index, Qt::CheckStateRole).toInt() == Qt::Checked) {
            res.append(this->data(index).toString());
        }
    }
    return res;
}

void CheckableModel::checkAll()
{
    setChecked(Qt::Checked);
}

void CheckableModel::uncheckAll()
{
    setChecked(Qt::Unchecked);
}
