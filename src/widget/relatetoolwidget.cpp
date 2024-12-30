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
    mChildName = {};
    mParentName = {};
}

void RelateToolWidget::push(const SName& table)
{

    if (mChildName.name.isEmpty()) {
        ui->childTable->setText(table.name);
        mChildName = table;
        return;
    }
    //QString parentTable = table;
    mParentName = table;
    ui->parentTable->setText(mParentName.name);
    emit selected(mParentName, mChildName);
}

void RelateToolWidget::on_clearParent_clicked()
{
    ui->parentTable->clear();
}


void RelateToolWidget::on_clearChild_clicked()
{
    ui->childTable->clear();
}
