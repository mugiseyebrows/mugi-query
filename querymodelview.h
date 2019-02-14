#ifndef QUERYMODELVIEW_H
#define QUERYMODELVIEW_H

#include <QWidget>

class QAbstractItemModel;
class QItemSelectionModel;

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

    QAbstractItemModel* model() const;

    QItemSelectionModel *selectionModel() const;
public slots:
    void updateSplitter();

protected slots:

    void on_tabs_currentChanged(int index);
    void onTableCustomContextMenuRequested(const QPoint &pos);

private:
    Ui::QueryModelView *ui;
    int mTabHeight;
    bool mSplitterUpdated;
};

#endif // QUERYMODELVIEW_H
