#ifndef RICHHEADERITERATOR_H
#define RICHHEADERITERATOR_H

#include "richheaderdirection.h"

class RichHeaderIterator {
public:
    RichHeaderIterator();

    RichHeaderIterator(RichHeaderDirection::DirectionType dir0, RichHeaderDirection::DirectionType dir1, int rowCount, int columnCount);

    QPair<int,int> next();

    void set(int row, int column);

    QPair<int, int> current() const;
protected:

    void first();

    int mRow;
    int mColumn;
    RichHeaderDirection::DirectionType mDir0;
    RichHeaderDirection::DirectionType mDir1;
    int mRowCount;
    int mColumnCount;
};

#endif // RICHHEADERITERATOR_H
