#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = nullptr);

signals:

    // QGraphicsView interface
protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
};

#endif // GRAPHICSVIEW_H
