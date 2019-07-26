#include "richheaderdata.h"

#include <QSet>
#include <QSize>
#include "richheaderiterator.h"
#include "richheadercell.h"

RichHeaderData::RichHeaderData() :
    mMultiline(true), mAlign(Qt::AlignCenter), mElide(Qt::ElideNone), mRotation(0.0)
{
}

RichHeaderData::~RichHeaderData()
{
    qDeleteAll(mCells.values());
}

RichHeaderCell RichHeaderData::cell(int row, int column)
{
    IntPair index(row,column);
    if (!mCells.contains(index)) {
        mCells[index] = new RichHeaderCellImpl(row,column,1,1,QString(),mMultiline,mElide,mAlign,mRotation,0);
    }
    return RichHeaderCell(mCells[index]);
}

RichHeaderData* RichHeaderData::multiline(bool value) {
    mMultiline = value;
    return this;
}

RichHeaderData* RichHeaderData::elide(Qt::TextElideMode value) {
    mElide = value;
    return this;
}

RichHeaderData* RichHeaderData::align(Qt::Alignment value) {
    mAlign = value;
    return this;
}

RichHeaderData* RichHeaderData::rotation(double value) {
    mRotation = value;
    return this;
}

RichHeaderCellList RichHeaderData::cells() const {
    return mCells.values();
}

QPair<int,int> RichHeaderData::spannedSections(int section) {
    int section1 = section;
    int section2 = section;
    RichHeaderCellList cells = this->cells(section);
    foreach(RichHeaderCellImpl* cell, cells) {
        section1 = qMin(section1, cell->column());
        section2 = qMax(section2, cell->column() + cell->columnSpan() - 1);
    }
    return QPair<int,int>(section1,section2);
}

//   a1 b1 b2 a2
//    b1 a1 b2
//    b1 a2 b2
bool RichHeaderData::rangesOverlap(int a1, int a2, int b1, int b2) {
    return (a1 <= b1 && a2 >= b2) || (a2 >= b1 && a2 <= b2) || (a1 >= b1 && a1 <= b2);
}

void RichHeaderData::testRangesOverlap() {

    bool passed = true;

    for(int i=0;i<1000;i++) {

        int a1 = rand() % 20;
        int a2 = a1 + (rand() % 20);
        int b1 = rand() % 20;
        int b2 = b1 + (rand() % 20);

        bool actual = rangesOverlap(a1,a2,b1,b2);
        QSet<int> a;
        QSet<int> b;

        for(int i=a1;i<=a2;i++) {
            a << i;
        }
        for(int i=b1;i<=b2;i++) {
            b << i;
        }

        bool expected = a.intersect(b).size() > 0;

        if (actual != expected) {
            qDebug() << "failed" << a1 << a2 << b1 << b2 << "expected" << expected << "got" << actual;
            passed = false;
        } else {
            //qDebug() << a1 << a2 << b1 << b2 << actual;
        }
    }

    qDebug() << "testRangesOverlap()" << (passed ? "passed" : "failed");
}

RichHeaderCellList RichHeaderData::widgetCellsOverlapsRange(int section1, int section2) {
    RichHeaderCellList result;
    foreach(RichHeaderCellImpl* cell, mCells) {
        if (rangesOverlap(cell->column(), cell->column() + cell->columnSpan() - 1, section1, section2)) {
            result.append(cell);
        }
    }
    return result;
}

QSet<int> RichHeaderData::cellsSections(const RichHeaderCellList &cells) {
    QSet<int> result;
    foreach(RichHeaderCellImpl* cell, cells) {
        int column = cell->column();
        int columnSpan = cell->columnSpan();
        for(int c=0;c<columnSpan;c++) {
            result.insert(column + c);
        }
    }
    return result;
}

RichHeaderCellList RichHeaderData::widgetCellsToTheRight(int section) {
    RichHeaderCellList cells = mCells.values();
    RichHeaderCellList res;
    foreach(RichHeaderCellImpl* cell, cells) {
        if (!cell->widget()) {
            continue;
        }
        int column = cell->column() + cell->columnSpan() - 1;
        if (column >= section) {
            res.append(cell);
        }
    }
    return res;
}

RichHeaderCellList RichHeaderData::cells(int section) const {
    RichHeaderCellList cells = mCells.values();
    RichHeaderCellList res;
    foreach(RichHeaderCellImpl* cell, cells) {
        int column1 = cell->column();
        int column2 = column1 + cell->columnSpan() - 1;
        if (column1 <= section && section <= column2) {
            res.append(cell);
        }
    }
    return res;
}

void RichHeaderData::grow(RichHeaderCellImpl* cell, RichHeaderCellBitmap& occupied, RichHeaderDirection::DirectionType direction) {

    //qDebug() << "grow" << direction << cell;
    if (direction == RichHeaderDirection::DirectionUp) {
        cell->incRowSpan();
        cell->moveUp();
    } else if (direction == RichHeaderDirection::DirectionRight) {
        cell->incColumnSpan();
    } else if (direction == RichHeaderDirection::DirectionDown) {
        cell->incRowSpan();
    } else if (direction == RichHeaderDirection::DirectionLeft) {
        cell->incColumnSpan();
        cell->moveLeft();
    }
    occupied.update(cell);
}

bool RichHeaderData::canGrow(RichHeaderCellImpl* cell, RichHeaderCellBitmap& occupied, RichHeaderDirection::DirectionType direction) {

    if (RichHeaderDirection::vertical(direction)) {

        int row;
        if (direction == RichHeaderDirection::DirectionDown) {
            row = cell->row() + cell->rowSpan();
        } else {
            row = cell->row() - 1;
        }
        for(int i=0;i<cell->columnSpan();i++) {
            int column = cell->column() + i;
            if (occupied.test(row,column)) {
                return false;
            }
        }
        return true;

    } else {

        int column;
        if (direction == RichHeaderDirection::DirectionRight) {
            column = cell->column() + cell->columnSpan();
        } else {
            column = cell->column() - 1;
        }
        for(int i=0;i<cell->rowSpan();i++) {
            int row = cell->row() + i;
            if (occupied.test(row,column)) {
                return false;
            }
        }
        return true;

    }

    return false;
}

QSize RichHeaderData::sizeHint() const
{
    int sum = std::accumulate(mSubsectionSizes.begin(),mSubsectionSizes.end(),0);
    return QSize(1,sum);
}

QList<int> RichHeaderData::subsectionSizes() const
{
    return mSubsectionSizes;
}

void RichHeaderData::subsectionSizes(const QList<int> sizes)
{
    mSubsectionSizes = sizes;
}

void RichHeaderData::pull(RichHeaderDirection::DirectionType direction, int sectionCount)
{
    if (mSubsectionSizes.isEmpty()) {
        qDebug() << "pull() after subsectionSizes(QList<int>)";
        return;
    }

    int subsectionCount = mSubsectionSizes.size();

    RichHeaderCellBitmap occupied(subsectionCount, sectionCount);
    RichHeaderCellList cells = mCells.values();
    foreach(RichHeaderCellImpl* cell, cells) {
        occupied.update(cell);
    }

    RichHeaderIterator it = RichHeaderIterator(direction,RichHeaderDirection::orthogonal(direction), occupied.rowCount(), occupied.columnCount());

    while (true) {
        QPair<int,int> rowColumn = it.next();
        int row = rowColumn.first;
        int column = rowColumn.second;
        if (row < 0 || column < 0) {
            break;
        }
        IntPair index(row,column);
        if (!mCells.contains(index)) {
            continue;
        }
        RichHeaderCellImpl* cell = mCells[index];
        while(canGrow(cell,occupied,direction)) {
            grow(cell,occupied,direction);
        }
    }

}



