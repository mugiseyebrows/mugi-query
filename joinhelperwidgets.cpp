#include "joinhelperwidgets.h"
#include "ui_joinhelperwidgets.h"

#include "joinhelperwidget.h"
#include "tokens.h"

#include <QSignalMapper>

JoinHelperWidgets::JoinHelperWidgets(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoinHelperWidgets)
{
    ui->setupUi(this);

    while(ui->tabs->count() > 0) {
        ui->tabs->removeTab(0);
    }

}

JoinHelperWidgets::~JoinHelperWidgets()
{
    delete ui;
}

JoinHelperWidget* JoinHelperWidgets::tab(int index) {
    return qobject_cast<JoinHelperWidget*>(ui->tabs->widget(index));
}

int JoinHelperWidgets::tabIndex(const QString& connectionName) {
    for(int i=0;i<ui->tabs->count();i++) {
        JoinHelperWidget* w = tab(i);
        if (w->connectionName() == connectionName) {
            return i;
        }
    }
    return -1;
}

void JoinHelperWidgets::update(const QString &connectionName, const Tokens& tokens, bool focus)
{
    int index = tabIndex(connectionName);
    JoinHelperWidget* w;
    if (index > -1) {
        w = tab(index);
    } else {
        if (!focus) {
            return;
        }
        w = new JoinHelperWidget();
        w->init(connectionName);
        index = ui->tabs->count();
        ui->tabs->insertTab(index,w,connectionName);
        connect(w,&JoinHelperWidget::appendQuery,[this,connectionName](QString query, bool newSession){
            emit appendQuery(connectionName,query,newSession);
        });
    }
    w->update(tokens);
    ui->tabs->setCurrentIndex(index);
}

void JoinHelperWidgets::closeTab(const QString &connectionName)
{
    int index = tabIndex(connectionName);
    if (index > -1) {
        ui->tabs->removeTab(index);
    }
}

void JoinHelperWidgets::save()
{
    for(int i=0;i<ui->tabs->count();i++) {
        tab(i)->saveRelationsModel();
    }
}


