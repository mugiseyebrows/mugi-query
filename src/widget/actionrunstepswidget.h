#ifndef ACTIONRUNSTEPSWIDGET_H
#define ACTIONRUNSTEPSWIDGET_H

#include <QWidget>
#include "action.h"

namespace Ui {
class ActionRunStepsWidget;
}

class ActionRunStepsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActionRunStepsWidget(QWidget *parent = nullptr);
    ~ActionRunStepsWidget();

    void init(const QList<Function>& functions);

signals:
    void completed();

protected:
    Ui::ActionRunStepsWidget *ui;
    QList<Function> mFunctions;
    int mIndex;
private slots:
    void on_exec_clicked();
};

#endif // ACTIONRUNSTEPSWIDGET_H
