#include "schema2relationdialog2.h"
#include "ui_schema2relationdialog2.h"

#include "schema2relation.h"
#include <QStandardItemModel>
#include "tablebuttons/tablebuttons.h"
#include "schema2tablemodel.h"

Schema2RelationDialog2::Schema2RelationDialog2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Schema2RelationDialog2)
{
    ui->setupUi(this);
}

Schema2RelationDialog2::~Schema2RelationDialog2()
{
    delete ui;
}

void Schema2RelationDialog2::init(Schema2TableModel *childTable, Schema2TableModel *parentTable, Schema2Relation *relation)
{
    mChildTable = childTable;
    mParentTable = parentTable;
    mRelation = relation;

    ui->childTable->setText(childTable->tableName());

    ui->parentTable->setText(parentTable->tableName());

    QStandardItemModel* model = new QStandardItemModel(relation->parentColumns().size(), 2);


    ui->columns->setModel(model);

}

