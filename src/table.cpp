#include "table.h"

#include <algorithm>
#include <QRect>
#include <QDebug>

static int intCeil(int n, int d) {
    return (n + d - 1) / d;
}

Table::Table()
{

}

Table::Table(const QRect &rect, int rowHeight, int spacing) : mRect(rect), mRowHeight(rowHeight), mSpacing(spacing) {
    mRowCount = rect.height() / mRowHeight;
}

void Table::insertColumn(int index, int width) {
    mColumns.insert(index, width);
    mTotalWidth = std::accumulate(mColumns.begin(), mColumns.end(), 0) + (mColumns.size() - 1) * mSpacing;
}

QRect Table::cell(int row, int column) const {
    int x0 = (mRect.width() - mTotalWidth) / 2;
    int x = x0;
    for(int c=0;c<column;c++) {
        x += mColumns[c] + mSpacing;
    }
    int y0 = (mRect.height() - mRowHeight * mRowCount) / 2;
    int y = y0 + mRowHeight * row;

    QRect rect(QPoint(x, y) + mRect.topLeft(), QSize(mColumns[column], mRowHeight));

#if 0
    //qDebug() << rect.topLeft() << rect.size();
    rect.adjust(-2,-2,2,2);
    //qDebug() << rect.topLeft() << rect.size();
#endif

    return rect;
}

int Table::totalWidth() const {
    return mTotalWidth;
}

int Table::availableWidth() const {
    return mRect.width() - mTotalWidth;
}

void Table::setRowCount(int value) {
    mRowCount = value;
}
