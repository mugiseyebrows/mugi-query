#ifndef COMPARETABLEWIDGET_H
#define COMPARETABLEWIDGET_H

#include <QWidget>

namespace Ui {
class CompareTableWidget;
}

class CompareTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CompareTableWidget(QWidget *parent = nullptr);
    ~CompareTableWidget();

private slots:
    void on_compare_clicked();

private:
    Ui::CompareTableWidget *ui;
};

#endif // COMPARETABLEWIDGET_H
