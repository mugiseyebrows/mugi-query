#ifndef RICHHEADERDATA_H
#define RICHHEADERDATA_H

#include <QString>
#include <QMap>
#include <QSet>
#include <QPair>

#include "richheaderdirection.h"
#include "richheadercellbitmap.h"
#include "richheadercell.h"
#include "richheadercellimpl.h"

class RichHeaderView;

typedef QList<RichHeaderCellImpl *> RichHeaderCellList;

class RichHeaderData
{
public:
    typedef QPair<int,int> IntPair;

    RichHeaderData();

    ~RichHeaderData();

    RichHeaderCell cell(int row, int column);

    void pull(RichHeaderDirection::DirectionType direction, int sectionCount);

    QSize sizeHint() const;
    QList<int> subsectionSizes() const;

    void subsectionSizes(const QList<int> sizes);

    int rowCount() const;

    bool canGrow(RichHeaderCellImpl *cell, RichHeaderCellBitmap &occupied, RichHeaderDirection::DirectionType direction);

    int cellIndex(int row, int column);

    void grow(RichHeaderCellImpl* cell, RichHeaderCellBitmap &occupied, RichHeaderDirection::DirectionType direction);
    RichHeaderCellList cells(int section) const;
    RichHeaderCellList cells() const;

    RichHeaderCellList widgetCellsToTheRight(int section = 0);
    QPair<int, int> spannedSections(int section);
    RichHeaderData *multiline(bool value);
    RichHeaderData *elide(Qt::TextElideMode value);
    RichHeaderData *align(Qt::Alignment value);
    RichHeaderData *rotation(double value);
    RichHeaderCellList widgetCellsOverlapsRange(int section1, int section2);
    static QSet<int> cellsSections(const RichHeaderCellList &cells);
    static bool rangesOverlap(int a1, int a2, int b1, int b2);
    static void testRangesOverlap();
protected:

    RichHeaderView* mView;
    QMap<IntPair,RichHeaderCellImpl*> mCells;
    QList<int> mSubsectionSizes;
    bool mMultiline;
    Qt::TextElideMode mElide;
    Qt::Alignment mAlign;
    double mRotation;

};

#endif // RICHHEADERDATA_H
