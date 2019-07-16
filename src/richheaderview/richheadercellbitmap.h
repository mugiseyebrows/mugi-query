#ifndef RICHHEADERCELLBITMAP_H
#define RICHHEADERCELLBITMAP_H

#include <QBitArray>
class RichHeaderCellImpl;

class RichHeaderCellBitmap {
public:
    RichHeaderCellBitmap(int rowCount, int columnCount);
    bool test(int row, int column) const;
    void update(RichHeaderCellImpl *cell);

    int rowCount() const;
    int columnCount() const;

protected:

    QBitArray mData;
    int mRowCount;
    int mColumnCount;
};

#endif // RICHHEADERCELLBITMAP_H
