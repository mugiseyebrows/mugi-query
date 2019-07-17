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

    void setModel(QAbstractItemModel *model) override;

    RichHeaderData* data() const;

public slots:
    void update();

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

    QModelIndex mCurrent;

    QBitArray mSelected;

    RichHeaderData* mHeaderData;

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
