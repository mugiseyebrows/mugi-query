#ifndef TABLE_H
#define TABLE_H

#include <QRect>
#include <QList>

class Table {
public:
    Table(const QRect& rect, int lineHeight, int spacing);

    void insertColumn(int index, int width);

    QRect cell(int row, int column) const;

    int totalWidth() const;

    int availableWidth() const;

    void setRowCount(int value);

protected:

    int mRowCount;
    int mTotalWidth;
    int mRowHeight;
    QList<int> mColumns;
    int mSpacing;
    QRect mRect;
};


#endif // TABLE_H
