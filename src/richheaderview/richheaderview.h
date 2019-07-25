#ifndef RICHHEADERVIEW_H
#define RICHHEADERVIEW_H

#include <QHeaderView>
#include <QBitArray>
#include "richheaderdata.h"

class RichHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    RichHeaderView(Qt::Orientation orientation, QWidget *parent = 0);

    void setModel(QAbstractItemModel *model);

    void setSelectionModel(QItemSelectionModel *selectionModel);

    RichHeaderData* data() const;

    QSize sizeHint() const;

    void setSizeHint(const QSize& size);

    void setHighlightColor(const QColor& color);

    void setFlatStyle(bool value);

public slots:
    void update();

    void onSelectionChanged(QItemSelection, QItemSelection);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

    QModelIndex mCurrent;

    RichHeaderData* mHeaderData;

    QSet<int> mHighlighted;
    QColor mHighlighColor;
    QSize mSizeHint;
    bool mFlatStyle;

    bool rowIntersectsSelection(int row) const;
    bool columnIntersectsSelection(int column) const;

    int sectionCount() const;
    bool sectionIntersectsSelection(int logical) const;
    bool isSectionSelected(int section) const;

    QRect cellRect(RichHeaderCellImpl *cell) const;
protected slots:

    void onSectionResized(int, int, int);
};

#endif // RICHHEADERVIEW_H
