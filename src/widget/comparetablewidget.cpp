#include "comparetablewidget.h"
#include "ui_comparetablewidget.h"


CompareTableWidget::CompareTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompareTableWidget)
{
    ui->setupUi(this);
}

CompareTableWidget::~CompareTableWidget()
{
    delete ui;
}

void CompareTableWidget::on_compare_clicked()
{
    QString database1 = ui->source1->database();
    QString table1 = ui->source1->table();
    QString database2 = ui->source2->database();
    QString table2 = ui->source2->table();


}

