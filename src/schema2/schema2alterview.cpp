#include "schema2alterview.h"
#include "ui_schema2alterview.h"

#include "schema2tablemodel.h"
#include "schema2relationmodel.h"
#include "schema2index.h"

// todo push schema for one table

Schema2AlterView::Schema2AlterView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Schema2AlterView)
{
    ui->setupUi(this);
}

Schema2AlterView::~Schema2AlterView()
{
    delete ui;
}

#include <QStandardItemModel>

void Schema2AlterView::init(Schema2TableModel *model,
                            const QList<Schema2RelationModel*>& relations) {

    ui->columns->setModel(model);
    ui->columns->hideColumn(Schema2TableModel::col_name);
    ui->columns->hideColumn(Schema2TableModel::col_type);

    QStandardItemModel* relationsModel = new QStandardItemModel(relations.size(), 1);

    for(int i=0;i<relations.size();i++) {
        relationsModel->setData(relationsModel->index(i, 0), relations[i]->parentTable());
    }

    ui->relations->setModel(relationsModel);

    QList<Schema2Index*> indexes = model->getIndexes();

    QStandardItemModel* indexesModel = new QStandardItemModel(indexes.size(), 1);

    for(int i=0;i<indexes.size();i++) {
        indexesModel->setData(indexesModel->index(i, 0), indexes[i]->name());
    }

    ui->indexes->setModel(indexesModel);
}

/*
void Schema2AlterView::setModel(Schema2TableModel *model)
{
    ui->table->setModel(model);
    ui->table->hideColumn(Schema2TableModel::col_name);
    ui->table->hideColumn(Schema2TableModel::col_type);
}
*/
