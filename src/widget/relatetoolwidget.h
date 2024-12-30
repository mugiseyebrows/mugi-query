#ifndef RELATETOOLWIDGET_H
#define RELATETOOLWIDGET_H

#include <QWidget>
#include "sdata.h"

namespace Ui {
class RelateToolWidget;
}

class RelateToolWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RelateToolWidget(QWidget *parent = nullptr);
    ~RelateToolWidget();

signals:
    void selected(SName, SName);

public slots:
    void clear();
    void push(const SName &);

protected:
    SName mChildName;
    SName mParentName;

private slots:
    void on_clearParent_clicked();

    void on_clearChild_clicked();

private:
    Ui::RelateToolWidget *ui;
};

#endif // RELATETOOLWIDGET_H
