#include "richheaderdata.h"

#include <QSet>
#include <QSize>
#include "richheaderiterator.h"
#include "richheadercell.h"

RichHeaderData::RichHeaderData() : mMultiline(true), mAlign(Qt::AlignCenter), mElide(Qt::ElideNone), mRotation(0.0)
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

QList<RichHeaderCellImpl*> RichHeaderData::cells() const {
    return mCells.values();
}

QPair<int,int> RichHeaderData::spannedSections(int section) {
    int section1 = section;
    int section2 = section;
    QList<RichHeaderCellImpl*> cells = this->cells(section);
    foreach(RichHeaderCellImpl* cell, cells) {
        section1 = qMin(section1, cell->column());
        section2 = qMax(section2, cell->column() + cell->columnSpan() - 1);
    }
    return QPair<int,int>(section1,section2);
}

QList<RichHeaderCellImpl*> RichHeaderData::widgetCellsToTheRight(int section) {
    QList<RichHeaderCellImpl*> cells = mCells.values();
    QList<RichHeaderCellImpl*> res;
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

QList<RichHeaderCellImpl*> RichHeaderData::cells(int section) const {
    QList<RichHeaderCellImpl*> cells = mCells.values();
    QList<RichHeaderCellImpl*> res;
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

void RichHeaderData::pull(RichHeaderDirection::DirectionType direction, int rowCount, int columnCount)
{

    RichHeaderCellBitmap occupied(rowCount, columnCount);
    QList<RichHeaderCellImpl*> cells = mCells.values();
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



