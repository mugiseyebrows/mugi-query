#ifndef CHECKABLEVIEW_H
#define CHECKABLEVIEW_H

#include <QWidget>

class QSortFilterProxyModel;
class CheckableModel;

namespace Ui {
class CheckableView;
}

class CheckableView : public QWidget
{
    Q_OBJECT

public:
    explicit CheckableView(QWidget *parent = nullptr,const QStringList& items = {});
    ~CheckableView();

    void append(const QStringList& items);

    QStringList checked() const;

public slots:
    void on_checkAll_clicked();
    void on_uncheckAll_clicked();
    void on_filter_textChanged(const QString &text);
private:
    Ui::CheckableView *ui;

    QSortFilterProxyModel* mProxyModel;
    CheckableModel* mModel;

};

#endif // CHECKABLEVIEW_H
