#ifndef ROWVALUEGETTER_H
#define ROWVALUEGETTER_H

#include <QVariant>
class QAbstractItemModel;

class RowValueGetter {
public:
    RowValueGetter(QAbstractItemModel* model, int row);
    virtual QVariant operator()(int col) ;
protected:
    QAbstractItemModel* mModel;
    int mRow;
};

#endif // ROWVALUEGETTER_H
