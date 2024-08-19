#include "relatetoolwidget.h"
#include "ui_relatetoolwidget.h"

RelateToolWidget::RelateToolWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RelateToolWidget)
{
    ui->setupUi(this);
}

RelateToolWidget::~RelateToolWidget()
{
    delete ui;
}

void RelateToolWidget::clear()
{
    ui->childTable->setText(QString());
    ui->parentTable->setText(QString());
}

void RelateToolWidget::push(QString table)
{
    QString childTable = ui->childTable->text();
    if (childTable.isEmpty()) {
        ui->childTable->setText(table);
        return;
    }
    QString parentTable = table;
    ui->parentTable->setText(parentTable);
    emit selected(parentTable, childTable);
}

void RelateToolWidget::on_clearParent_clicked()
{
    ui->parentTable->clear();
}


void RelateToolWidget::on_clearChild_clicked()
{
    ui->childTable->clear();
}
