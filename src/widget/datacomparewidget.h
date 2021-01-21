#ifndef DATACOMPAREWIDGET_H
#define DATACOMPAREWIDGET_H

#include <QWidget>
class QSqlQueryModel;

namespace Ui {
class DataCompareWidget;
}

class DataCompareWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataCompareWidget(QWidget *parent = nullptr);
    ~DataCompareWidget();

    void setModels(QSqlQueryModel *model1, QSqlQueryModel *model2);
public slots:
    void onColumnsChanged();
    void onModeChanged();
private:
    Ui::DataCompareWidget *ui;
};

#endif // DATACOMPAREWIDGET_H
