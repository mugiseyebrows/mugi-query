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

class RichHeaderData
{
public:
    typedef QPair<int,int> IntPair;

    RichHeaderData();

    ~RichHeaderData();

    RichHeaderCell cell(int row, int column);

    void pull(RichHeaderDirection::DirectionType direction, int rowCount, int columnCount);

    QSize sizeHint() const;
    QList<int> subsectionSizes() const;

    void subsectionSizes(const QList<int> sizes);

    int rowCount() const;

    bool canGrow(RichHeaderCellImpl *cell, RichHeaderCellBitmap &occupied, RichHeaderDirection::DirectionType direction);

    int cellIndex(int row, int column);

    void grow(RichHeaderCellImpl* cell, RichHeaderCellBitmap &occupied, RichHeaderDirection::DirectionType direction);
    QList<RichHeaderCellImpl *> cells(int section) const;
    QList<RichHeaderCellImpl *> cells() const;

    QList<RichHeaderCellImpl *> widgetCellsToTheRight(int section = 0);
    QPair<int, int> spannedSections(int section);
    RichHeaderData *multiline(bool value);
    RichHeaderData *elide(Qt::TextElideMode value);
    RichHeaderData *align(Qt::Alignment value);
    RichHeaderData *rotation(double value);
protected:

    QMap<IntPair,RichHeaderCellImpl*> mCells;
    QList<int> mSubsectionSizes;
    bool mMultiline;
    Qt::TextElideMode mElide;
    Qt::Alignment mAlign;
    double mRotation;

};

#endif // RICHHEADERDATA_H
