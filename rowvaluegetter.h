#ifndef ROWVALUEGETTER_H
#define ROWVALUEGETTER_H

#include <QVariant>
class QAbstractItemModel;

class RowValueGetter {
public:
    RowValueGetter(const QAbstractItemModel* model, int row);
    virtual ~RowValueGetter();
    virtual QVariant operator()(int col) const ;
protected:
    const QAbstractItemModel* mModel;
    int mRow;
};

#endif // ROWVALUEGETTER_H
