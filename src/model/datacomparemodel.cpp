#include "datacomparemodel.h"

#include <QDebug>
#include <QColor>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include "datautils.h"

namespace  {

QString modelQuery(QAbstractItemModel *model) {
    QSqlQueryModel* model_ = qobject_cast<QSqlQueryModel*>(model);
    if (!model_) {
        return QString();
    }
    return model_->query().lastQuery();
}
}

DataCompareModel::DataCompareModel(QObject *parent) :
    QAbstractTableModel(parent), mColumnCount(0), mMode(Full)
{

}

void DataCompareModel::setModels(QAbstractItemModel *model1, QAbstractItemModel *model2)
{
    mModels = {model1, model2};

    /*qDebug() << "modelQuery"
             << modelQuery(model1)
             << modelQuery(model2);*/

    update();
}

void DataCompareModel::setKeyColumns(const QList<int> &columns)
{
    mKeyColumns = columns;
    update();
}

QVariantList DataCompareModel::rowKey(QAbstractItemModel* model, int row) const {
    QVariantList key;
    QList<int> keyColumns = mKeyColumns;
    if (keyColumns.isEmpty()) {
        for(int column=0;column<model->columnCount();column++) {
            keyColumns.append(column);
        }
    }
    for(int column: keyColumns) {
        key.append(model->data(model->index(row, column)));
    }
    return key;
}

int DataCompareModel::findRow(QAbstractItemModel* model, const QVariantList& key) const {
    for(int row=0;row<model->rowCount();row++) {
        QVariantList key_ = rowKey(model, row);
        if (key == key_) {
            return row;
        }
    }
    return -1;
}

bool DataCompareModel::testKey(QAbstractItemModel* model, const QVariantList& key) const {

    int count = 0;
    for(int row=0;row<model->rowCount();row++) {
        QVariantList key_ = rowKey(model, row);
        if (key == key_) {
            count++;
        }
    }
    return count < 2;
}

void DataCompareModel::setKeys(const QList<QVariantList>& keys, const QList<QMap<int,int>>& rows) {

    if (mColumnCount == 0) {
        int columnCount = mModels[0]->columnCount();
        int first = 0;
        int last = columnCount - 1;
        beginInsertColumns(QModelIndex(), first, last);
        mColumnCount = columnCount;
        endInsertColumns();
    }

    if (!mKeys.isEmpty()) {
        int first = 0;
        int last = mKeys.size() - 1;
        beginRemoveRows(QModelIndex(), first, last);
        mKeys.clear();
        endRemoveRows();
    }

    if (!keys.isEmpty()) {
        int first = 0;
        int last = keys.size() - 1;
        beginInsertRows(QModelIndex(), first, last);
        mKeys = keys;
        mRows = rows;
        endInsertRows();
    }

}


void DataCompareModel::update()
{
    if(mModels.isEmpty()) {
        return;
    }

    QList<QVariantList> keys;

    for(QAbstractItemModel* model: mModels) {
        for(int row=0;row<model->rowCount();row++) {
            QVariantList key = rowKey(model, row);
            if (keys.indexOf(key) < 0) {
                keys.append(key);
            }
        }
    }

    if (!mKeyColumns.isEmpty()) {
        for(QAbstractItemModel* model: mModels) {
            for(int i=0;i<keys.size();i++) {
                if (!testKey(model,keys[i])) {
                    return;
                }
            }
        }
    }

    QList<QMap<int,int>> m;
    m.append(QMap<int,int>());
    m.append(QMap<int,int>());
    for(int i=0;i<keys.size();i++) {
        QVariantList key = keys[i];
        m[0][i] = findRow(mModels[0], key);
        m[1][i] = findRow(mModels[1], key);
    }

    setKeys(keys, m);
}

void DataCompareModel::setMode(DataCompareModel::Mode mode)
{
    if (mMode == mode) {
        return;
    }
    mMode = mode;
    emit dataChanged(index(0,0),index(rowCount()-1,columnCount()-1));
}

int DataCompareModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mKeys.size();
}

int DataCompareModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return mColumnCount;
}

QVariantList DataCompareModel::valuesAt(int row_, int column) const {
    QVariantList res;
    for(int i=0;i<2;i++) {
        int row = mRows[i][row_];
        QAbstractItemModel* model = mModels[i];
        res.append(model->data(model->index(row, column)));
    }
    return res;
}

QVariant DataCompareModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && !mModels.isEmpty()) {
        return mModels[0]->headerData(section, orientation, role);
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant DataCompareModel::valueAt(int row_, int column) const {
    int row1 = mRows[0][row_];
    int row2 = mRows[1][row_];
    QAbstractItemModel* model;
    int row;
    if (row2 > -1) {
        row = row2;
        model = mModels[1];
    } else if (row1 > -1) {
        row = row1;
        model = mModels[0];
    } else {
        return QVariant();
    }
    return model->data(model->index(row, column));
}

bool DataCompareModel::isInsertedRow(int row) const {
    int row1 = mRows[0][row];
    int row2 = mRows[1][row];
    return row2 > -1 && row1 < 0;
}

bool DataCompareModel::modelContainsRow(int model, int row) const {
    return mRows[model][row] > -1;
}

bool DataCompareModel::isRemovedRow(int row) const {
    int row1 = mRows[0][row];
    int row2 = mRows[1][row];
    return row1 > -1 && row2 < 0;
}

QVariant lastNonNull(const QVariantList& vs) {
    for(int i=vs.size()-1;i>=0;i--) {
        if (!vs[i].isNull()) {
            return vs[i];
        }
    }
    return QVariant();
}

QString toString(const QVariant& v) {
    if (v.isNull()) {
        return QString();
    }
    return v.toString();
}

QVariant DataCompareModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    int column = index.column();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {

        QVariantList vs = valuesAt(row, column);
        QVariant v1 = vs[0];
        QVariant v2 = vs[1];

        if (mMode == Before) {
            return v1;
        } else if (mMode == After) {
            return v2;
        } else if (mMode == Full) {
            if (isInsertedRow(row) || isRemovedRow(row)) {
                return lastNonNull(vs);
            }
            if (v1 != v2) {
                return vs;
            }
            return v2;
        } else if (mMode == Short) {
            return lastNonNull(vs);
        }

    } else if (role == Qt::ForegroundRole) {
        if (isInsertedRow(row)) {
            return QColor(Qt::green);
        } else if (isRemovedRow(row)) {
            return QColor(Qt::red);
        } else {
            QVariantList vs = valuesAt(row, column);
            QVariant v1 = vs[0];
            QVariant v2 = vs[1];
            if (v1.isNull() && !v2.isNull()) {
                return QColor(Qt::green);
            }
            if (!v1.isNull() && v2.isNull()) {
                return QColor(Qt::red);
            }
            if (v1 != v2) {
                return QColor(Qt::blue);
            }
        }
    } else if (role == Qt::BackgroundRole) {
        if (mMode == Before && !modelContainsRow(0, row)) {
            return QColor("#E2E2E2");
        } else if (mMode == After && !modelContainsRow(1, row)) {
            return QColor("#E2E2E2");
        }
    }

    return QVariant();
}
