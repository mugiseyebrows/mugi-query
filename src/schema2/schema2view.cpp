#include "schema2view.h"
#include "ui_schema2view.h"

Schema2View::Schema2View(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Schema2View)
{
    ui->setupUi(this);
}

Schema2View::~Schema2View()
{
    delete ui;
}

void Schema2View::setScene(QGraphicsScene *scene)
{
    ui->view->setScene(scene);
}

void Schema2View::on_move_clicked(bool checked)
{

}

void Schema2View::on_relate_clicked(bool checked)
{

}

void Schema2View::on_alter_clicked(bool checked)
{

}

void Schema2View::on_insert_clicked(bool checked)
{

}

void Schema2View::on_create_clicked()
{

}

