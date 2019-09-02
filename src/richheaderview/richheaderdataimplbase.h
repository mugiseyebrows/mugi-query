#ifndef RICHHEADERDATAIMPLBASE_H
#define RICHHEADERDATAIMPLBASE_H

#include "richheadercellbitmap.h"
#include "richheaderdirection.h"
#include "richheadercellimpl.h"
#include "richheadercell.h"
#include <QSet>

class RichHeaderDataImplBase
{
public:
    RichHeaderDataImplBase();
    virtual ~RichHeaderDataImplBase();

    static void grow(RichHeaderCellImpl* cell, RichHeaderCellBitmap& occupied,
                     RichHeaderDirection::DirectionType direction);
    static bool canGrow(RichHeaderCellImpl* cell, RichHeaderCellBitmap& occupied,
                        RichHeaderDirection::DirectionType direction);
    void pull(RichHeaderDirection::DirectionType direction, int sectionCount);
    RichHeaderCellList cells(int section);
    RichHeaderCellList widgetCellsToTheRight(int section);
    static QSet<int> cellsSections(const RichHeaderCellList &cells);
    RichHeaderCellList widgetCellsOverlapsRange(int section1, int section2);
    static bool rangesOverlap(int a1, int a2, int b1, int b2);
    QPair<int, int> spannedSections(int section);
    QSize sizeHint() const;
    bool hasCell(int row, int column) const;
protected:
    QList<int> mSubsectionSizes;
    QMap<QPair<int,int> , RichHeaderCellImpl*> mCells;
};

#endif // RICHHEADERDATAIMPLBASE_H
