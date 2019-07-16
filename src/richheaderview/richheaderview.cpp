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

}

RichHeaderView::RichHeaderView(Qt::Orientation orientation, QWidget *parent) :
    QHeaderView(orientation,parent), mHeaderData(new RichHeaderData())
{
    //viewport()->setMouseTracking(true);
    setSectionsClickable(true);
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
    QTimer::singleShot(0,this,SLOT(onTimeout()));
}

void RichHeaderView::onTimeout() {
    //QList<RichHeaderCellImpl*> cells = mHeaderData->cells();
    /*foreach(RichHeaderCellImpl* cell, cells) {
        if (cell->widget()) {
            onSectionResized(cell->column(),0,0);
        }
    }*/
    update();
    showWidgets();
}

RichHeaderData *RichHeaderView::data() const
{
    return mHeaderData;
}

void RichHeaderView::update()
{
    onSectionResized(0,0,0);
}

void RichHeaderView::showWidgets()
{
     QList<RichHeaderCellImpl*> cells = mHeaderData->widgetCellsToTheRight(0);
     foreach(RichHeaderCellImpl* cell, cells) {
         cell->widget()->show();
     }
}

QRect RichHeaderView::cellRect(RichHeaderCellImpl* cell) const{
    int row = cell->row();
    int column = cell->column();
    int rowSpan = cell->rowSpan();
    int columnSpan = cell->columnSpan();
    int left = sectionViewportPosition(column);
    int top = 0;

    QList<int> subsectionSizes = mHeaderData->subsectionSizes();

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
        height += subsectionSizes.value(row + i,0);
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

        style()->drawControl(QStyle::CE_Header, &opt, painter, this);

        if (!drawText) {

            double rotation = cell->rotation();

            QMatrix original = painter->matrix();

            QFontMetrics metrics(this->font());

            QPoint center = cellRect.center();
            QRect textRect = cellRect;

            QString text = cell->text();

            if (equals(rotation,90.0) || equals(rotation,270.0)) {
                textRect = rotate90(cellRect);
            } else if (equals(rotation,0) || equals(rotation,180.0)) {

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

        //qDebug() << cellRect;

    }

    if (cells.isEmpty()) {
        style()->drawControl(QStyle::CE_HeaderEmptyArea, &opt, painter, this);
    }

}
