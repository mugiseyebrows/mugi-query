#include "xjoinmodel.h"

#include <QSqlQueryModel>
#include <QSqlRecord>
#include "fieldnames.h"
#include "hash.h"

static StringHash<int> toFields(QSqlQueryModel *model) {
    StringHash<int> res;
    QStringList names = fieldNames(model->record());
    for(int i=0;i<names.size();i++) {
        res.set(names[i], i);
    }
    return res;
}

static QList<int> toIndexes(const StringHash<int>& fields, const QStringList& columns) {
    QList<int> res;
    for(const QString& column: columns) {
        if (fields.contains(column)) {
            res.append(fields.get(column));
        } else {
            res.append(-1);
        }
    }
    return res;
}

XJoinModel::XJoinModel(QSqlQueryModel *model1, const QStringList &columns1, QSqlQueryModel *model2, const QStringList &columns2, QObject *parent)
    : mModel1(model1), mModel2(model2), QAbstractTableModel{parent}
{

    QList<int> indexes1 = toIndexes(toFields(model1), columns1);
    QList<int> indexes2 = toIndexes(toFields(model2), columns2);

    if (indexes1.contains(-1) || indexes2.contains(-1)) {
        return;
    }

    for(int row1=0;row1<model1->rowCount();row1++) {
        for(int row2=0;row2<model1->rowCount();row2++) {
            bool m = true;
            for(int k=0;k<indexes1.size();k++) {
                QVariant v1 = model1->data(model1->index(row1, indexes1[k]));
                QVariant v2 = model2->data(model2->index(row2, indexes2[k]));
                if (v1.isNull() || v2.isNull()) {
                    m = false;
                    break;
                }
                if (v1 != v2) {
                    m = false;
                    break;
                }
            }
            if (m) {
                mMatched.append({row1, row2});
            }
        }
    }


}

int XJoinModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return mMatched.size();
}

int XJoinModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mModel1->columnCount() + mModel2->columnCount();
}

QVariant XJoinModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        int column = index.column();
        int column_ = column - mModel1->columnCount();
        QList<int> m = mMatched[index.row()];
        if (column < mModel1->columnCount()) {
            return mModel1->data(mModel1->index(m[0], column));
        } else {
            return mModel2->data(mModel2->index(m[1], column_));
        }
    }
    return QVariant();
}

QVariant XJoinModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section < mModel1->columnCount()) {
            return mModel1->headerData(section, orientation, role);
        } else {
            int section_ = section - mModel1->columnCount();
            return mModel2->headerData(section_, orientation, role);
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}
