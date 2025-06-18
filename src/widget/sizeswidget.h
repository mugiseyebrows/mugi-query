#ifndef SIZESWIDGET_H
#define SIZESWIDGET_H

#include <QWidget>

namespace Ui {
class SizesWidget;
}

class SizesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SizesWidget(QWidget *parent = nullptr);
    ~SizesWidget();

private slots:
    void on_rowSize_currentIndexChanged(int index);

    void on_colSize_currentIndexChanged(int index);

private:
    Ui::SizesWidget *ui;
};

#endif // SIZESWIDGET_H
