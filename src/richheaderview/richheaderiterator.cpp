#include "richheaderiterator.h"

namespace {

int maybeReverse(int index, int size, RichHeaderDirection::DirectionType dir) {
    if (index < 0) {
        return index;
    }
    return RichHeaderDirection::ascending(dir) ? index : size - index - 1;
}

}

RichHeaderIterator::RichHeaderIterator()
{
}

RichHeaderIterator::RichHeaderIterator(RichHeaderDirection::DirectionType dir0, RichHeaderDirection::DirectionType dir1, int rowCount, int columnCount) {
    mDir0 = dir0;
    mDir1 = dir1;
    mRowCount = rowCount;
    mColumnCount = columnCount;
    first();
}

void RichHeaderIterator::first() {
    mRow = 0;
    mColumn = 0;
    if (RichHeaderDirection::horizontal(mDir0)) {
        mColumn--;
    } else {
        mRow--;
    }
}

QPair<int, int> RichHeaderIterator::current() const {
    return QPair<int,int>(maybeReverse(mRow,mRowCount,RichHeaderDirection::horizontal(mDir0,mDir1)),
                          maybeReverse(mColumn,mColumnCount,RichHeaderDirection::vertical(mDir0,mDir1)));
}

QPair<int, int> RichHeaderIterator::next() {

    if (RichHeaderDirection::horizontal(mDir0)) {
        mColumn++;
        if (mColumn >= mColumnCount) {
            mRow++;
            mColumn = 0;
            if (mRow >= mRowCount) {
                mRow = -1;
                mColumn = -1;
            }
        }
    } else {
        mRow++;
        if (mRow >= mRowCount) {
            mColumn++;
            mRow = 0;
            if (mColumn >= mColumnCount) {
                mRow = -1;
                mColumn = -1;
            }
        }
    }
    return current();
}

void RichHeaderIterator::set(int row, int column)
{
    mRow = maybeReverse(row, mRowCount, RichHeaderDirection::horizontal(mDir0,mDir1));
    mColumn = maybeReverse(column, mColumnCount, RichHeaderDirection::horizontal(mDir0,mDir1));
}
