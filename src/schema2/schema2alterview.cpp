#include "schema2alterview.h"
#include "ui_schema2alterview.h"

#include "schema2tablemodel.h"
#include "schema2relationmodel.h"
#include "schema2index.h"
#include <QStandardItemModel>
#include "schema2relationslistmodel2.h"
#include "tablebuttons.h"
#include "itemdelegatewithcompleter.h"

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

void Schema2AlterView::initColumns(Schema2TableModel *model, const QStringList& types) {
    ui->columns->setModel(model);
    ui->columns->hideColumn(Schema2TableModel::col_name);
    ui->columns->hideColumn(Schema2TableModel::col_type);

    TableButtons* buttons = new TableButtons();

    const int button_remove = 0;
    const int button_insert = 1;

    buttons->button(button_remove).inside().text("-").size(40, 40);
    buttons->button(button_insert).between().text("+").size(40, 40).offset(40, 0);

    bool canInsert = false;

    buttons->setView(ui->columns);
    connect(buttons, &TableButtons::clicked, [=](int id, int index){
        if (id == button_remove) {
            if (index < 0) {
                return;
            }
            model->removeRow(index);
        } else if (id == button_insert) {
            int row = canInsert ? index : model->rowCount();
            model->insertRow(row);
        }
    });

    auto* delegate = new ItemDelegateWithCompleter(types, ui->columns);
    ui->columns->setItemDelegateForColumn(Schema2TableModel::col_newtype, delegate);


}

void Schema2AlterView::initRelations(Schema2TableModel *model) {
    QList<Schema2Relation*> relations = model->getRelations().values();
    Schema2RelationsListModel2* relationsModel = new Schema2RelationsListModel2(relations);
    ui->relations->setModel(relationsModel);

    TableButtons* buttons = new TableButtons();
    buttons->button(0).inside().text("-").size(40, 40);
    buttons->button(1).between().text("+").size(40, 40).offset(40, 0);

    buttons->setView(ui->relations);
    connect(buttons, &TableButtons::clicked, [](int id, int index){

    });

}

void Schema2AlterView::initIndexes(Schema2TableModel *model) {
    QList<Schema2Index*> indexes = model->getIndexes();
    QStandardItemModel* indexesModel = new QStandardItemModel(indexes.size(), 1);
    for(int i=0;i<indexes.size();i++) {
        indexesModel->setData(indexesModel->index(i, 0), indexes[i]->name());
    }
    ui->indexes->setModel(indexesModel);

    TableButtons* buttons = new TableButtons();
    buttons->button(0).inside().text("-").size(40, 40);
    buttons->button(1).between().text("+").size(40, 40).offset(40, 0);
    buttons->setView(ui->indexes);
    connect(buttons, &TableButtons::clicked, [](int id, int index){

    });

}

void Schema2AlterView::init(Schema2TableModel *model, const QStringList& types) {
    initColumns(model, types);
    initRelations(model);
    initIndexes(model);
}

/*
void Schema2AlterView::setModel(Schema2TableModel *model)
{
    ui->table->setModel(model);
    ui->table->hideColumn(Schema2TableModel::col_name);
    ui->table->hideColumn(Schema2TableModel::col_type);
}
*/

void Schema2AlterView::on_createRelation_clicked()
{

}


void Schema2AlterView::on_createIndex_clicked()
{

}

