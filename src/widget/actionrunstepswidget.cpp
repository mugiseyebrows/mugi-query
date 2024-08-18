#include "actionrunstepswidget.h"
#include "ui_actionrunstepswidget.h"
#include <QStringListModel>
#include <QListView>

ActionRunStepsWidget::ActionRunStepsWidget(QWidget *parent)
    : mIndex(0), QWidget(parent)
    , ui(new Ui::ActionRunStepsWidget)
{
    ui->setupUi(this);
}

ActionRunStepsWidget::~ActionRunStepsWidget()
{
    delete ui;
}

void ActionRunStepsWidget::init(const QList<Function> &functions)
{
    mFunctions = functions;
    QStringList names;
    for(int i=0;i<mFunctions.size();i++) {
        names.append(mFunctions[i].name);
    }
    QStringListModel* model = new QStringListModel(names);
    ui->view->setModel(model);
    ui->view->setCurrentIndex(model->index(mIndex, 0));
}

void ActionRunStepsWidget::on_exec_clicked()
{
    auto item = mFunctions[mIndex];
    item.fn();
    mIndex += 1;
    if (mIndex >= mFunctions.size()) {
        emit completed();
        close();
        return;
    }
    auto* model = ui->view->model();
    ui->view->setCurrentIndex(model->index(mIndex, 0));
}

