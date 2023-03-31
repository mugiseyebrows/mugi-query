#ifndef DATACOMPAREWIDGET2_H
#define DATACOMPAREWIDGET2_H

#include <QWidget>

namespace Ui {
class DataCompareWidget2;
}

class DataCompareWidget2 : public QWidget
{
    Q_OBJECT

public:
    explicit DataCompareWidget2(QWidget *parent = nullptr);
    ~DataCompareWidget2();

private:
    Ui::DataCompareWidget2 *ui;
};

#endif // DATACOMPAREWIDGET2_H
