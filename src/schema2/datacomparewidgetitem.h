#ifndef DATACOMPAREWIDGETITEM_H
#define DATACOMPAREWIDGETITEM_H

#include <QWidget>

namespace Ui {
class DataCompareWidgetItem;
}

class DataCompareWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit DataCompareWidgetItem(QWidget *parent = nullptr);
    ~DataCompareWidgetItem();

private:
    Ui::DataCompareWidgetItem *ui;
};

#endif // DATACOMPAREWIDGETITEM_H
