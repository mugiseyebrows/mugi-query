#include "schema2view.h"
#include "ui_schema2view.h"

#include "schema2data.h"
#include <QDebug>

Schema2View::Schema2View(QWidget *parent) :
    mMode(ModeNone),
    mData(0),
    QWidget(parent),
    ui(new Ui::Schema2View)
{
    ui->setupUi(this);
}

Schema2View::~Schema2View()
{
    delete ui;
}

void Schema2View::setData(Schema2Data *data)
{
    mData = data;

    connect(mData,SIGNAL(tableClicked(QString)),this,SLOT(onTableClicked(QString)));

    ui->view->setScene(mData->scene());
}

void Schema2View::onTableClicked(QString tableName)
{

    //qDebug() << "onTableClicked" << tableName << mMode;

    switch (mMode) {
    case ModeNone: break;
    case ModeMove: break;
    case ModeRelate:
        mRelationTables.append(tableName);
        if (mRelationTables.size() == 2) {
            QString childTable = mRelationTables[0];
            QString parentTable = mRelationTables[1];
            if (childTable != parentTable) {
                mData->showRelationDialog(childTable, parentTable, this);
            }
            mRelationTables.clear();
        }
        break;
    case ModeAlter: mData->showAlterView(tableName); break;
    case ModeInsert: mData->showInsertView(tableName); break;
    }
}

void Schema2View::uncheckAllExcept(QPushButton* checked) {

    QList<QPushButton*> buttons = {ui->move, ui->relate, ui->alter, ui->insert};
    for(QPushButton* button: buttons) {
        if (button == checked) {
            continue;
        }
        button->setChecked(false);
    }

}

void Schema2View::on_move_clicked(bool checked)
{
    uncheckAllExcept(ui->move);
    mMode = checked ? ModeMove : ModeNone;
}

void Schema2View::on_relate_clicked(bool checked)
{
    uncheckAllExcept(ui->relate);
    mMode = checked ? ModeRelate : ModeNone;
    mRelationTables.clear();
}

void Schema2View::on_alter_clicked(bool checked)
{
    uncheckAllExcept(ui->alter);
    mMode = checked ? ModeAlter : ModeNone;
}

void Schema2View::on_insert_clicked(bool checked)
{
    uncheckAllExcept(ui->insert);
    mMode = checked ? ModeInsert : ModeNone;
}

void Schema2View::on_create_clicked()
{

}

void Schema2View::on_arrange_clicked()
{

}

void Schema2View::on_pull_clicked()
{
    mData->pull();
}

void Schema2View::on_push_clicked()
{
    mData->push();
}

