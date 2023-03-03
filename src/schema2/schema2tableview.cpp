#include "schema2tableview.h"
#include "ui_schema2tableview.h"



Schema2TableView::Schema2TableView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Schema2TableView)
{
    ui->setupUi(this);
}

Schema2TableView::~Schema2TableView()
{
    delete ui;
}

void Schema2TableView::setModel(Schema2TableModel *model)
{

}
