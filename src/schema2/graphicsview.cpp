#include "graphicsview.h"

#include "style.h"

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView{parent}
{
    setBackgroundBrush(Style::current.BackgroundColor);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);
    //setCacheMode(QGraphicsView::CacheBackground);
}


void GraphicsView::drawBackground(QPainter *painter, const QRectF &r)
{
    QGraphicsView::drawBackground(painter, r);

    auto drawGrid = [&](double gridStep) {
        QRect windowRect = rect();
        QPointF tl = mapToScene(windowRect.topLeft());
        QPointF br = mapToScene(windowRect.bottomRight());

        double left = std::floor(tl.x() / gridStep - 0.5);
        double right = std::floor(br.x() / gridStep + 1.0);
        double bottom = std::floor(tl.y() / gridStep - 0.5);
        double top = std::floor(br.y() / gridStep + 1.0);

        // vertical lines
        for (int xi = int(left); xi <= int(right); ++xi) {
            QLineF line(xi * gridStep, bottom * gridStep, xi * gridStep, top * gridStep);

            painter->drawLine(line);
        }

        // horizontal lines
        for (int yi = int(bottom); yi <= int(top); ++yi) {
            QLineF line(left * gridStep, yi * gridStep, right * gridStep, yi * gridStep);
            painter->drawLine(line);
        }
    };

    QPen pfine(Style::current.FineGridColor, 1.0);

    painter->setPen(pfine);
    drawGrid(15);

    QPen p(Style::current.CoarseGridColor, 1.0);

    painter->setPen(p);
    drawGrid(150);
}
