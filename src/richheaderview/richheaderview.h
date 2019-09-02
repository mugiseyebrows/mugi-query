#ifndef RICHHEADERVIEW_H
#define RICHHEADERVIEW_H

#include <QHeaderView>
#include <QBitArray>
#include "richheaderdata.h"
#include "richheaderdataimpl.h"

class RichHeaderTabFilter;

class RichHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    RichHeaderView(Qt::Orientation orientation, QWidget *parent = 0);
    void setModel(QAbstractItemModel *model);
    void setSelectionModel(QItemSelectionModel *selectionModel);
    RichHeaderData data();
    QSize sizeHint() const;

public slots:
    void update();

    void onSelectionChanged(QItemSelection, QItemSelection);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;
    QPair<int, int> cellOf(QWidget *widget);
    QPair<int, int> nextWidgetCellWithTabFocus(const QPair<int, int> &cell, RichHeaderDirection::DirectionType dir1, RichHeaderDirection::DirectionType dir2);


    QModelIndex mCurrent;

    RichHeaderDataImpl* mHeaderData;

    QSet<int> mHighlighted;

    RichHeaderTabFilter* mTabFilter;

    bool rowIntersectsSelection(int row) const;
    bool columnIntersectsSelection(int column) const;

    int sectionCount() const;
    bool sectionIntersectsSelection(int logical) const;
    bool isSectionSelected(int section) const;

    QRect cellRect(RichHeaderCellImpl *cell) const;

protected slots:

    void onSectionResized(int, int, int);
    void onWidgetTabPressed(bool shift);
};

#endif // RICHHEADERVIEW_H
