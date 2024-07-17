#include "richheaderview.h"

#include "richheaderdataimpl.h"

#include <QDebug>
#include <QVariantList>
#include <QPainter>
#include <QTextOption>
#include <QColor>
#include "richheaderdataimpl.h"
#include <QMouseEvent>
#include <QStyle>
#include <math.h>
#include <QTimer>
#include "richheadertextfitter.h"
#include "richheadertabfilter.h"
#include "richheaderiterator.h"

namespace {

    bool equals(double a, double b) {
        return fabs(a-b) < 0.00001;
    }

    QRect rotate90(const QRect& rect) {
        QRect res(QPoint(0,0),QSize(rect.height(),rect.width()));
        res.moveCenter(rect.center());
        return res;
    }

    double normalize360(double value) {
        while (value < 0) {
            value += 360.0;
        }
        while (value > 360.0) {
            value -= 360.0;
        }
        return value;
    }


    QSet<int> selectionColumns(const QItemSelection& sel) {
        QSet<int> result;
        foreach(const QItemSelectionRange& rng, sel) {
            if (!rng.isValid()) {
                continue;
            }
            int c1 = rng.topLeft().column();
            int c2 = rng.bottomRight().column();
            for(int c=c1;c<=c2;c++) {
                result.insert(c);
            }
        }
        return result;
    }

    QSet<int> selectionRows(const QItemSelection& sel) {
        QSet<int> result;
        foreach(const QItemSelectionRange& rng, sel) {
            if (!rng.isValid()) {
                continue;
            }
            int r1 = rng.topLeft().row();
            int r2 = rng.bottomRight().row();
            for(int r=r1;r<=r2;r++) {
                result.insert(r);
            }
        }
        return result;
    }


    int minElement(const QSet<int>& vs) {
        return *std::min_element(vs.begin(),vs.end());
    }

    int maxElement(const QSet<int>& vs) {
        return *std::max_element(vs.begin(),vs.end());
    }


}

RichHeaderView::RichHeaderView(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation,parent), mHeaderData(new RichHeaderDataImpl()), mTabFilter(new RichHeaderTabFilter(this))
{
    //viewport()->setMouseTracking(true);

#if QT_VERSION >= 0x050000
    setSectionsClickable(true);
#else
    setClickable(true);
#endif

    connect(this,SIGNAL(sectionResized(int,int,int)),this,SLOT(onSectionResized(int,int,int)));
    connect(mTabFilter,SIGNAL(tabPressed(bool)),this,SLOT(onWidgetTabPressed(bool)));
}

int RichHeaderView::sectionCount() const {
    if (!model()) {
        return 0;
    }
    if (orientation() == Qt::Horizontal) {
        return model()->columnCount();
    }
    return model()->rowCount();
}

void RichHeaderView::setModel(QAbstractItemModel *model)
{
    QHeaderView::setModel(model);
    QTimer::singleShot(0,this,SLOT(update()));
}

void RichHeaderView::setSelectionModel(QItemSelectionModel *selectionModel)
{
    //qDebug() << "setSelectionModel" << selectionModel;
    QHeaderView::setSelectionModel(selectionModel);

    if (selectionModel) {
        connect(selectionModel,
                SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this,
                SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
    }
}


void RichHeaderView::onSelectionChanged(QItemSelection,QItemSelection desel) {
    if (!mHeaderData->highlightColor().isValid()) {
        return;
    }
    if (orientation() == Qt::Horizontal) {
        mHighlighted = selectionColumns(selectionModel()->selection());
    } else {
        mHighlighted = selectionRows(selectionModel()->selection());
    }
    //update();

    // repaint deselected sections
    QSet<int> cols = selectionColumns(desel);
    if (cols.isEmpty()) {
        return;
    }
    int section1 = minElement(cols);
    int section2 = maxElement(cols);

    RichHeaderCellList cells = mHeaderData->widgetCellsOverlapsRange(section1,section2);
    QSet<int> sections = RichHeaderDataImpl::cellsSections(cells);
    section1 = minElement(sections);
    section2 = maxElement(sections);
    for(int index=section1;index<=section2;index++) {
        updateSection(index);
    }

}

RichHeaderData RichHeaderView::data()
{
    return RichHeaderData(mHeaderData);
}

QSize RichHeaderView::sizeHint() const
{
    return mHeaderData->sizeHint();
}

void RichHeaderView::update()
{
    onSectionResized(0,0,0);
}

QRect RichHeaderView::cellRect(RichHeaderCellImpl* cell) const{
    int row = cell->row();
    int column = cell->column();
    int rowSpan = cell->rowSpan();
    int columnSpan = cell->columnSpan();
    int left = sectionViewportPosition(column);
    int top = 0;

    QSize sizeHint = this->sizeHint();

    QList<int> subsectionSizes = mHeaderData->subsectionSizes();

    int lastRowSize = sizeHint.height() - std::accumulate(subsectionSizes.begin(),subsectionSizes.end() - 1,0);

    for(int i=0;i<row;i++) {
        top += subsectionSizes.value(i,0);
        if (i >= subsectionSizes.size()) {
            qDebug() << "RichHeaderView::sectionRect top error";
        }
    }
    int width = 0;
    for(int i=0;i<columnSpan;i++) {
        width += sectionSize(column + i);
    }
    int height = 0;
    for(int i=0;i<rowSpan;i++) {
        if (row + i == subsectionSizes.size() - 1) {
            height += lastRowSize;
        } else {
            height += subsectionSizes.value(row + i, 0);
        }
        if (row + i >= subsectionSizes.size()) {
            qDebug() << "RichHeaderView::sectionRect height error";
        }
    }
    return QRect(left,top,width,height);
}

void RichHeaderView::onSectionResized(int logical,int,int) {

    RichHeaderCellList cells = mHeaderData->widgetCellsToTheRight(logical);

    QWidget* viewport = this->viewport();

    bool stretchFixed = mHeaderData->stretchFixed();

    foreach(RichHeaderCellImpl* cell, cells) {
        QRect rect = cellRect(cell);

        int paddingTop = cell->paddingTop();
        int paddingRight = cell->paddingRight();
        int paddingBottom = cell->paddingBottom();
        int paddingLeft = cell->paddingLeft();

        rect.setWidth(rect.width() - paddingRight - paddingLeft);
        rect.setHeight(rect.height() - paddingTop - paddingBottom);
        rect.moveTo(rect.topLeft() - QPoint(1,1) + QPoint(paddingLeft,paddingTop));

        QWidget* widget = cell->widget();

        if (!stretchFixed) {
            QSizePolicy policy = widget->sizePolicy();
            if (policy.verticalPolicy() == QSizePolicy::Fixed || policy.horizontalPolicy() == QSizePolicy::Fixed) {
                QPoint p = rect.center();
                QSize hint = widget->sizeHint();
                QSize orig = rect.size();
                if (policy.verticalPolicy() == QSizePolicy::Fixed) {
                    rect.setHeight(hint.height());
                }
                if (policy.horizontalPolicy() == QSizePolicy::Fixed) {
                    rect.setWidth(hint.width());
                }
                Qt::Alignment align = cell->align();
                int shiftY = 0;
                int shiftX = 0;

                if (align != Qt::AlignCenter) {
                    int halfHeightDiff = (orig.height() - hint.height()) / 2;
                    int halfWidthDiff = (orig.width() - hint.width()) / 2;
                    shiftX += (align & Qt::AlignLeft ? -halfWidthDiff : 0) +
                            (align & Qt::AlignRight ? halfWidthDiff : 0);
                    shiftY += (align & Qt::AlignTop ? -halfHeightDiff : 0) +
                            (align & Qt::AlignBottom ? halfHeightDiff : 0);
                }

                rect.moveCenter(p + QPoint(shiftX, shiftY));
            }
        }

        widget->setGeometry(rect);
        widget->setVisible(cell->visible());
        widget->setParent(viewport);
        mTabFilter->watch(widget);
    }

    QPair<int,int> spanned = mHeaderData->spannedSections(logical);
    for(int section=spanned.first;section <= spanned.second; section++) {
        if (section != logical) {
            updateSection(section);
        }
    }

}

bool RichHeaderView::isSectionSelected(int section) const
{
    if (!selectionModel()) {
        return false;
    }
    if (orientation() == Qt::Horizontal) {
        return selectionModel()->isColumnSelected(section,rootIndex());
    }
    return selectionModel()->isRowSelected(section,rootIndex());
}

void RichHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{

    int visual = visualIndex(logicalIndex);

    QStyleOptionHeader opt;
    opt.initFrom(this);

    QStyle::State state = QStyle::State_None | QStyle::State_Raised;
    if (isEnabled()) {
        state |= QStyle::State_Enabled;
    }
    if (window()->isActiveWindow()) {
        state |= QStyle::State_Active;
    }

    opt.section = logicalIndex;
    opt.rect = rect;

    painter->setClipRect(rect);

    RichHeaderCellList cells = mHeaderData->cells(logicalIndex);

    auto original = painter->transform();

    foreach(RichHeaderCellImpl* cell, cells) {

        if (cell->widget()) {
            continue;
        }

        QRect cellRect = this->cellRect(cell);

        painter->setClipRect(cellRect);

        cellRect.moveTo(cellRect.topLeft() - QPoint(1,1));

        painter->setFont(this->font());

        bool selected = true;
        for(int column = cell->column(); column < cell->column() + cell->columnSpan(); column++) {
            if (!isSectionSelected(column)) {
                selected = false;
            }
        }

        if (selected) {
            state |= QStyle::State_Sunken;
            state |= QStyle::State_On;
        } else {
            state &= ~QStyle::State_Sunken;
            state &= ~QStyle::State_On;
        }

        opt.state = state;

        bool drawText = equals(cell->rotation(),0.0) && !cell->multiline() && cell->elide() == Qt::ElideNone;

        opt.rect = cellRect;
        opt.text = drawText ? cell->text() : QString();
        opt.textAlignment = cell->align();

        //opt.position = QStyleOptionHeader::Middle;
        opt.orientation = orientation();

        bool previousSelected = isSectionSelected(this->logicalIndex(visual - 1));
        bool nextSelected = isSectionSelected(this->logicalIndex(visual + 1));
        if (previousSelected && nextSelected)
            opt.selectedPosition = QStyleOptionHeader::NextAndPreviousAreSelected;
        else if (previousSelected)
            opt.selectedPosition = QStyleOptionHeader::PreviousIsSelected;
        else if (nextSelected)
            opt.selectedPosition = QStyleOptionHeader::NextIsSelected;
        else
            opt.selectedPosition = QStyleOptionHeader::NotAdjacent;

        if (mHeaderData->flatStyle()) {
            painter->fillRect(cellRect,Qt::white);
            painter->setPen(QColor("#A0A0A0"));
            painter->drawRect(cellRect);
        } else {
            style()->drawControl(QStyle::CE_Header, &opt, painter, this);
        }

        if (mHighlighted.contains(logicalIndex)) {
            QBrush brush(mHeaderData->highlightColor());
            painter->fillRect(cellRect,brush);
        }

        if (!drawText) {

            double rotation = normalize360(cell->rotation());

            QFontMetrics metrics(this->font());

            QPoint center = cellRect.center();
            QRect textRect;

            QString text = cell->text();

            if (equals(rotation,90.0) || equals(rotation,270.0)) {
                textRect = rotate90(cellRect);
            } else if (equals(rotation,0) || equals(rotation,180.0)) {
                textRect = cellRect;
            } else {
                if (cell->multiline()) {
                    textRect = RichHeaderTextFitter::fitMultiline(cellRect,metrics.height(),rotation,3.0);
                } else if (cell->elide() != Qt::ElideNone) {
                    textRect = RichHeaderTextFitter::fit(cellRect,metrics.height(),rotation),metrics.height();
                    text = metrics.elidedText(text,cell->elide(),textRect.width());
                } else {
                    textRect = RichHeaderTextFitter::fit(cellRect,metrics.height(),rotation);
                }
            }

            textRect.moveCenter(center);

            int align = cell->align();
            if (cell->multiline()) {
                align |= Qt::TextWordWrap;
            }

            auto rotated = QTransform();
            rotated.translate(center.x(), center.y());
            rotated.rotate(rotation);
            rotated.translate(-center.x(), -center.y());

            painter->setTransform(rotated);

            style()->drawItemText(painter, textRect, align, this->palette(), isEnabled(), text, QPalette::WindowText);

            painter->setTransform(original);
        }

    }

    if (cells.isEmpty()) {
        style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, painter, this);
    }

}

QPair<int,int> RichHeaderView::cellOf(QWidget* widget) {
    RichHeaderCellList cells = mHeaderData->widgetCellsToTheRight(0);
    foreach(RichHeaderCellImpl* cell, cells) {
        if (cell->widget() == widget) {
            return QPair<int,int>(cell->row(),cell->column());
        }
    }
    return QPair<int,int>(-1,-1);
}



QPair<int,int> RichHeaderView::nextWidgetCellWithTabFocus(const QPair<int,int>& cell,
                                                          RichHeaderDirection::DirectionType dir1,
                                                          RichHeaderDirection::DirectionType dir2) {

    RichHeaderDataImpl* data = mHeaderData;
    int subsectionCount = data->subsectionSizes().size();
    int sectionCount = this->sectionCount();

    RichHeaderIterator it(dir1, dir2, subsectionCount, sectionCount);
    it.set(cell.first, cell.second);
    QPair<int,int> current = it.next();
    while(current.first > -1) {
        //qDebug() << "1" << current;
        if (data->hasCell(current.first, current.second)
                && data->cell(current.first, current.second).widget()
                && data->cell(current.first, current.second).widget()->focusPolicy() & Qt::TabFocus) {
            return current;
        }
        current = it.next();
    }
    it.set(RichHeaderDirection::ascending(RichHeaderDirection::vertical(dir1, dir2)) ? 0 : subsectionCount - 1,
           RichHeaderDirection::ascending(RichHeaderDirection::horizontal(dir1, dir2)) ? 0 : sectionCount - 1);

    current = it.current();
    while(current.first > -1 && current != cell) {
        //qDebug() << "2" << current;
        if (data->hasCell(current.first, current.second)
                && data->cell(current.first, current.second).widget()
                && data->cell(current.first, current.second).widget()->focusPolicy() & Qt::TabFocus) {
            return current;
        }
        current = it.next();
    }

    return QPair<int,int>(-1,-1);
}

void RichHeaderView::onWidgetTabPressed(bool shift) {
    qDebug() << "onWidgetTabPressed";

    QWidget* widget = mTabFilter->widget();

    QPair<int,int> index = cellOf(widget);
    if (index.first < 0) {
        qDebug() << "index.first < 0" << index << widget;
        return;
    }

    Qt::Orientation tabDirection = mHeaderData->tabDirection();

    RichHeaderDirection::DirectionType dir1 =
            tabDirection == Qt::Horizontal
            ? RichHeaderDirection::maybeInverted(RichHeaderDirection::DirectionRight, shift)
            : RichHeaderDirection::maybeInverted(RichHeaderDirection::DirectionDown, shift);
    RichHeaderDirection::DirectionType dir2 =
            tabDirection != Qt::Horizontal
            ? RichHeaderDirection::maybeInverted(RichHeaderDirection::DirectionRight, shift)
            : RichHeaderDirection::maybeInverted(RichHeaderDirection::DirectionDown, shift);

    QPair<int,int> nextCell = nextWidgetCellWithTabFocus(index, dir1, dir2);
    if (nextCell.first < 0) {
        qDebug() << "no next cell";
        return;
    }

    QWidget* nextWidget = mHeaderData->cell(nextCell.first, nextCell.second).widget();
    //nextWidget->focusWidget();
    nextWidget->setFocus();

}

