#include "dataimportwidgets.h"
#include "ui_dataimportwidgets.h"

#include "dataimportwidget.h"
#include "showandraise.h"

DataImportWidgets::DataImportWidgets(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataImportWidgets)
{
    ui->setupUi(this);

    while(ui->tabs->count() > 0) {
        ui->tabs->removeTab(0);
    }

}

DataImportWidgets::~DataImportWidgets()
{
    delete ui;
}

DataImportWidget* DataImportWidgets::tab(int index) {
    return qobject_cast<DataImportWidget*>(ui->tabs->widget(index));
}

int DataImportWidgets::tabIndex(const QString& connectionName) {
    for(int i=0;i<ui->tabs->count();i++) {
        DataImportWidget* w = tab(i);
        if (w->connectionName() == connectionName) {
            return i;
        }
    }
    return -1;
}

void DataImportWidgets::closeTab(const QString &connectionName)
{
    int index = tabIndex(connectionName);
    if (index > -1) {
        ui->tabs->removeTab(index);
    }
}

void DataImportWidgets::create(const QString &connectionName, const Tokens& tokens) {
    DataImportWidget* widget = new DataImportWidget();
    widget->init(connectionName);
    connect(widget,&DataImportWidget::appendQuery,[=](QString query){
        emit appendQuery(connectionName,query);
    });
    connect(this,SIGNAL(updateTokens(QString,Tokens)),widget,SLOT(onUpdateTokens(QString,Tokens)));
    emit updateTokens(connectionName,tokens);
    showAndRaise(widget);
}

void DataImportWidgets::update(const QString &connectionName, const Tokens& tokens)
{
#if 0
    int index = tabIndex(connectionName);
    DataImportWidget* w;
    if (index > -1) {
        w = tab(index);
    } else {
        if (!focus) {
            return;
        }
        w = new DataImportWidget();
        w->init(connectionName);
        index = ui->tabs->count();
        ui->tabs->insertTab(index,w,connectionName);
        connect(w,&DataImportWidget::appendQuery,[=](QString query){
            emit appendQuery(connectionName,query);
        });
    }
    w->update(tokens);
    ui->tabs->setCurrentIndex(index);
#endif

    emit updateTokens(connectionName,tokens);
}
