#include "richheaderview.h"

#include "richheaderdata.h"

#include <QDebug>
#include <QVariantList>
#include <QPainter>
#include <QTextOption>
#include <QColor>
#include "richheaderdata.h"
#include <QMouseEvent>
#include <QStyle>
#include <math.h>
#include <QTimer>
#include "richheadertextfitter.h"

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

}

RichHeaderView::RichHeaderView(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation,parent), mHeaderData(new RichHeaderData()), mFlatStyle(false)
{
    //viewport()->setMouseTracking(true);

#if QT_VERSION >= 0x050000
    setSectionsClickable(true);
#else
    setClickable(true);
#endif

    connect(this,SIGNAL(sectionResized(int,int,int)),this,SLOT(onSectionResized(int,int,int)));
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


void RichHeaderView::onSelectionChanged(QItemSelection,QItemSelection) {
    if (!mHighlighColor.isValid()) {
        return;
    }
    if (orientation() == Qt::Horizontal) {
        mHighlighted = selectionColumns(selectionModel()->selection());
    } else {
        mHighlighted = selectionRows(selectionModel()->selection());
    }
    update();
}

RichHeaderData *RichHeaderView::data() const
{
    return mHeaderData;
}

QSize RichHeaderView::sizeHint() const
{
    return mSizeHint.isValid() ? mSizeHint : QHeaderView::sizeHint();
}

void RichHeaderView::setSizeHint(const QSize &size)
{
    mSizeHint = size;
}

void RichHeaderView::setHighlightColor(const QColor &color)
{
    mHighlighColor = color;
    if (!color.isValid()) {
        mHighlighted.clear();
    }
}

void RichHeaderView::setFlatStyle(bool value)
{
    mFlatStyle = value;
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
            qDebug() << "MultilineHeaderView::sectionRect top error";
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
            qDebug() << "MultilineHeaderView::sectionRect height error";
        }
    }
    return QRect(left,top,width,height);
}

void RichHeaderView::onSectionResized(int logical,int,int) {

    QList<RichHeaderCellImpl*> cells = mHeaderData->widgetCellsToTheRight(logical);

    foreach(RichHeaderCellImpl* cell, cells) {
        QRect rect = cellRect(cell);
        rect.moveTo(rect.topLeft() - QPoint(1,1));
        cell->widget()->setGeometry(rect);
        cell->widget()->setVisible(cell->visible());
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
    opt.init(this);

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

    QList<RichHeaderCellImpl*> cells = mHeaderData->cells(logicalIndex);

    QMatrix original = painter->matrix();

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

        if (mFlatStyle) {
            painter->fillRect(cellRect,Qt::white);
            painter->setPen(QColor("#A0A0A0"));
            painter->drawRect(cellRect);
        } else {
            style()->drawControl(QStyle::CE_Header, &opt, painter, this);
        }

        if (mHighlighted.contains(logicalIndex)) {
            QBrush brush(mHighlighColor);
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

            QMatrix rotated = QMatrix();
            rotated.translate(center.x(), center.y());
            rotated.rotate(rotation);
            rotated.translate(-center.x(), -center.y());

            painter->setMatrix(rotated);

            style()->drawItemText(painter, textRect, align, this->palette(), isEnabled(), text, QPalette::WindowText);

            painter->setMatrix(original);
        }

    }

    if (cells.isEmpty()) {
        style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, painter, this);
    }

}
