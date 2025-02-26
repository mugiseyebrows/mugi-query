#include "table.h"

#include <algorithm>
#include <QRect>

static int intCeil(int n, int d) {
    return (n + d - 1) / d;
}

Table::Table(const QRect &rect, int lineHeight, int spacing) : mRect(rect), mRowHeight(lineHeight), mSpacing(spacing) {
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
    return QRect(QPoint(x, y) + mRect.topLeft(), QSize(mColumns[column], mRowHeight));
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
