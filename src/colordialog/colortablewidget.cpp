#include "colortablewidget.h"

#include <QResizeEvent>
#include <QPainter>

#define CELL_WIDTH 24

ColorTableWidget::ColorTableWidget(QWidget *parent)
    : QWidget{parent}
{}

void ColorTableWidget::setColors(const QList<QColor> &colors)
{
    mColors = colors;
    update();
}

static void drawGrid(QPainter* painter, int rows, int columns, int columnWidth, int rowHeight) {

    for(int row=0; row <= rows; row++) {
        double x1 = 0;
        double x2 = columns * columnWidth;
        double y1 = row * rowHeight;
        double y2 = y1;
        painter->drawLine(QPointF(x1, y1), QPoint(x2, y2));
    }
    for(int column = 0; column <= columns; column++) {
        double x1 = column * columnWidth;
        double y1 = 0;
        double x2 = x1;
        double y2 = rows * rowHeight;
        painter->drawLine(QPointF(x1, y1), QPoint(x2, y2));
    }

}


void ColorTableWidget::paintEvent(QPaintEvent *event)
{

    if (mColors.isEmpty()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    for(int i=0;i<mColors.size();i++) {
        int row = i / mColumns;
        int column = i % mColumns;
        painter.setBrush(mColors[i]);
        QRect rect(QPoint(column * CELL_WIDTH, row * CELL_WIDTH), QSize(CELL_WIDTH, CELL_WIDTH));
        painter.drawRect(rect);
    }

    painter.setPen(QPen(Qt::gray, 0.5));
    painter.setBrush(Qt::NoBrush);
    //drawGrid(&painter, mRows, mColumns, CELL_WIDTH, CELL_WIDTH);

}


void ColorTableWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = event->position().toPoint();
    int x = pos.x() / CELL_WIDTH;
    int y = pos.y() / CELL_WIDTH;
    int ix = x + y * mColumns;
    if (ix > -1 && ix < mColors.size()) {
        emit colorClicked(mColors[ix]);
    }
}

void ColorTableWidget::mouseReleaseEvent(QMouseEvent *event)
{
}

void ColorTableWidget::onResize() {

    if (mColumnsFixed > -1) {
        mColumns = mColumnsFixed;
    } else {
        mColumns = width() / CELL_WIDTH;
    }
    if (mRowsFixed > -1) {
        mRows = mRowsFixed;
    } else {
        mRows = height() / CELL_WIDTH;
    }
    update();
}

void ColorTableWidget::resizeEvent(QResizeEvent *event)
{
    onResize();
}


QSize ColorTableWidget::sizeHint() const
{
    return QSize(28 * CELL_WIDTH + 10, 11 * CELL_WIDTH + 10);
}


QSize ColorTableWidget::minimumSizeHint() const
{
    return QSize(100,100);
}
