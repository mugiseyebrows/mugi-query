#ifndef COLORTABLEWIDGET_H
#define COLORTABLEWIDGET_H

#include <QWidget>

class ColorTableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ColorTableWidget(QWidget *parent = nullptr);

    void setColors(const QList<QColor>& colors);

    void setColumns(int value) {
        mColumnsFixed = value;
        onResize();
    }
    void setRows(int value) {
        mRowsFixed = value;
        onResize();
    }

signals:
    void colorClicked(QColor);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;

    QList<QColor> mColors;
    int mColumns = 0;
    int mRows = 0;

    int mColumnsFixed = -1;
    int mRowsFixed = -1;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
    void onResize();

    // QWidget interface
public:
    QSize sizeHint() const override;

    // QWidget interface
public:
    QSize minimumSizeHint() const override;
};

#endif // COLORTABLEWIDGET_H
