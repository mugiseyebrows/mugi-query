#include "dataimportwidgets.h"
#include "ui_dataimportwidgets.h"

#include "dataimportwidget.h"

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


void DataImportWidgets::update(const QString &connectionName, const Tokens& tokens, bool focus)
{
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
}
