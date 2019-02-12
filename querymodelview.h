#ifndef QUERYMODELVIEW_H
#define QUERYMODELVIEW_H

#include <QWidget>

class QAbstractItemModel;

namespace Ui {
class QueryModelView;
}

class QueryModelView : public QWidget
{
    Q_OBJECT

public:
    explicit QueryModelView(QWidget *parent = nullptr);
    ~QueryModelView();

    void setModel(QAbstractItemModel* model);

    bool hasCurves() const;

private slots:
    void on_tabs_currentChanged(int index);

private:
    Ui::QueryModelView *ui;
    int mTabHeight;
};

#endif // QUERYMODELVIEW_H
