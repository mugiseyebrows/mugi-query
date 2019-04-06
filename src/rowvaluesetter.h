#ifndef ROWVALUESETTER_H
#define ROWVALUESETTER_H

#include <QVariant>
class QAbstractItemModel;

class RowValueSetter {
public:
    RowValueSetter(QAbstractItemModel* model, int row);
    virtual bool operator()(int col, const QVariant& value, int role = Qt::EditRole) ;
protected:
    QAbstractItemModel* mModel;
    int mRow;
};

class RowValueNotEmptySetter {
public:
    RowValueNotEmptySetter(QAbstractItemModel* model, int row);
    virtual bool operator()(int col, const QString& value, int role = Qt::EditRole) ;
protected:
    QAbstractItemModel* mModel;
    int mRow;
};

#endif // ROWVALUESETTER_H
