#include "richheaderdataimplbase.h"
#include "richheaderiterator.h"
#include <QSize>

RichHeaderDataImplBase::RichHeaderDataImplBase()
{

}

RichHeaderDataImplBase::~RichHeaderDataImplBase()
{
    qDeleteAll(mCells.values());
}

void RichHeaderDataImplBase::grow(RichHeaderCellImpl* cell, RichHeaderCellBitmap& occupied,
                              RichHeaderDirection::DirectionType direction) {

    // qDebug() << "grow" << direction << cell;
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

bool RichHeaderDataImplBase::canGrow(RichHeaderCellImpl* cell, RichHeaderCellBitmap& occupied,
                                 RichHeaderDirection::DirectionType direction) {

    if (RichHeaderDirection::vertical(direction)) {

        int row;
        if (direction == RichHeaderDirection::DirectionDown) {
            row = cell->row() + cell->rowSpan();
        } else {
            row = cell->row() - 1;
        }
        for (int i = 0; i < cell->columnSpan(); i++) {
            int column = cell->column() + i;
            if (occupied.test(row, column)) {
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
        for (int i = 0; i < cell->rowSpan(); i++) {
            int row = cell->row() + i;
            if (occupied.test(row, column)) {
                return false;
            }
        }
        return true;
    }

    return false;
}

void RichHeaderDataImplBase::pull(RichHeaderDirection::DirectionType direction,
                                  int sectionCount) {

    if (mSubsectionSizes.isEmpty()) {
        qDebug() << "pull() after subsectionSizes(QList<int>)";
        return;
    }

    int subsectionCount = mSubsectionSizes.size();

    RichHeaderCellBitmap occupied(subsectionCount, sectionCount);
    RichHeaderCellList cells = mCells.values();
    foreach (RichHeaderCellImpl* cell, cells) { occupied.update(cell); }

    RichHeaderIterator it =
        RichHeaderIterator(direction, RichHeaderDirection::orthogonal(direction),
                           occupied.rowCount(), occupied.columnCount());

    while (true) {
        QPair<int, int> rowColumn = it.next();
        int row = rowColumn.first;
        int column = rowColumn.second;
        if (row < 0 || column < 0) {
            break;
        }
        QPair<int,int> index(row, column);
        if (!mCells.contains(index)) {
            continue;
        }
        RichHeaderCellImpl* cell = mCells[index];
        while (canGrow(cell, occupied, direction)) {
            grow(cell, occupied, direction);
        }
    }

}

RichHeaderCellList RichHeaderDataImplBase::cells(int section) {
    RichHeaderCellList cells = mCells.values();
    RichHeaderCellList res;
    foreach (RichHeaderCellImpl* cell, cells) {
        int column1 = cell->column();
        int column2 = column1 + cell->columnSpan() - 1;
        if (column1 <= section && section <= column2) {
            res.append(cell);
        }
    }
    return res;
}

RichHeaderCellList RichHeaderDataImplBase::widgetCellsToTheRight(int section) {
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

QSet<int> RichHeaderDataImplBase::cellsSections(const RichHeaderCellList &cells) {
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


bool RichHeaderDataImplBase::rangesOverlap(int a1, int a2, int b1, int b2) {

    // return (a1 <= b1 && a2 >= b2) || (a2 >= b1 && a2 <= b2) || (a1 >= b1 && a1 <= b2);
    return !((a2 < b1) || (a1 > b2));
}

RichHeaderCellList RichHeaderDataImplBase::widgetCellsOverlapsRange(int section1, int section2) {
    RichHeaderCellList result;
    foreach(RichHeaderCellImpl* cell, mCells) {
        if (rangesOverlap(cell->column(), cell->column() + cell->columnSpan() - 1, section1, section2)) {
            result.append(cell);
        }
    }
    return result;
}

QPair<int,int> RichHeaderDataImplBase::spannedSections(int section) {
    int section1 = section;
    int section2 = section;
    RichHeaderCellList cells = this->cells(section);
    foreach(RichHeaderCellImpl* cell, cells) {
        section1 = qMin(section1, cell->column());
        section2 = qMax(section2, cell->column() + cell->columnSpan() - 1);
    }
    return QPair<int,int>(section1,section2);
}


QSize RichHeaderDataImplBase::sizeHint() const {

    int sum = std::accumulate(mSubsectionSizes.begin(), mSubsectionSizes.end(), 0);
    return QSize(1, sum);
}


bool RichHeaderDataImplBase::hasCell(int row, int column) const {

    return mCells.contains(QPair<int,int>(row, column));
}
