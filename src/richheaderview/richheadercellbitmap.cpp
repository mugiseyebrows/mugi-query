#include "richheadercellbitmap.h"
#include "richheadercellimpl.h"

RichHeaderCellBitmap::RichHeaderCellBitmap(int rowCount, int columnCount) : mRowCount(rowCount), mColumnCount(columnCount)
{
    mData.fill(false, mRowCount * mColumnCount);
}

bool RichHeaderCellBitmap::test(int row, int column) const
{
    int n = row * mColumnCount + column;
    if (n >= 0 && n < mData.size()) {
        return mData.testBit(n);
    }
    return true;
}

void RichHeaderCellBitmap::update(RichHeaderCellImpl *cell)
{
    for(int row=cell->row(); row<cell->row() + cell->rowSpan();row++) {
        for(int column=cell->column();column<cell->column() + cell->columnSpan();column++) {
            int bit = row * mColumnCount + column;
            if (bit >= mData.size() || bit < 0) {
                qDebug() << "update" << row << column << "outside bitmap" << mData.size();
            } else {
                mData.setBit(bit);
            }
        }
    }
}

int RichHeaderCellBitmap::rowCount() const
{
    return mRowCount;
}

int RichHeaderCellBitmap::columnCount() const
{
    return mColumnCount;
}
