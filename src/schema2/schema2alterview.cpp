#include "schema2alterview.h"
#include "ui_schema2alterview.h"

#include "schema2tablemodel.h"
#include "schema2relationmodel.h"
#include "schema2index.h"
#include <QStandardItemModel>
#include "tablebuttons.h"
#include "itemdelegatewithcompleter.h"
#include <QMessageBox>
#include "setcompleter.h"
#include "schema2data.h"
#include "schema2indexesmodel.h"
#include "schema2relationsmodel.h"
#include "schema2tablesmodel.h"

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

void Schema2AlterView::initColumns() {

    auto* model = mModel;

    ui->columns->setModel(model);
    ui->columns->hideColumn(Schema2TableModel::col_name);
    ui->columns->hideColumn(Schema2TableModel::col_type);

    TableButtons* buttons = new TableButtons();

    const int button_remove = 0;
    const int button_insert = 1;

    buttons->button(button_remove).inside().text("-").size(40, 40).offset(40, 0);
    buttons->button(button_insert).between().text("+").size(40, 40);

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

    auto* delegate = new ItemDelegateWithCompleter(mTypes, ui->columns);
    ui->columns->setItemDelegateForColumn(Schema2TableModel::col_newtype, delegate);


    ui->childTable->setText(model->tableName());

    setCompleter(ui->parentTable, mTables->tableNames());
}

void Schema2AlterView::initRelations() {

    ui->relations->setModel(mModel->relations());

    const int button_edit = 0;
    const int button_remove = 1;

    TableButtons* buttons = new TableButtons();
    buttons->button(button_edit).inside().text("mod").size(40, 40);
    buttons->button(button_remove).inside().text("-").size(40, 40).offset(40, 0);

    buttons->setView(ui->relations);
    connect(buttons, &TableButtons::clicked, [=](int id, int index){
        if (id == button_edit) {
            if (index > -1) {
                mData->editRelation(mModel->tableName(), mModel->relations()->at(index), this);
            }
        } else if (id == button_remove) {

        }
    });

}



void Schema2AlterView::initIndexes() {

    //auto* model = mModel;

#if 0
    QList<Schema2Index*> indexes = model->getIndexes();
    QStandardItemModel* indexesModel = new QStandardItemModel(indexes.size(), 1);
    for(int i=0;i<indexes.size();i++) {
        indexesModel->setData(indexesModel->index(i, 0), indexes[i]->name());
    }
    ui->indexes->setModel(indexesModel);
#endif

    ui->indexes->setModel(mModel->indexes());

    const int button_remove = 0;

    TableButtons* buttons = new TableButtons();
    buttons->button(button_remove).inside().text("-").size(40, 40);
    //buttons->button(1).between().text("+").size(40, 40).offset(40, 0);
    buttons->setView(ui->indexes);
    connect(buttons, &TableButtons::clicked, [=](int id, int index){
        if (id == button_remove) {
            if (index > -1) {
                mModel->indexes()->removeAt(index);
            }
        }
    });

}

void Schema2AlterView::init(Schema2Data *data, Schema2TablesModel* tableModels,
                            Schema2TableModel *model, const QStringList& types) {
    mData = data;
    mTables = tableModels;
    mModel = model;
    mTypes = types;
    initColumns();
    initRelations();
    initIndexes();
}

/*
void Schema2AlterView::setModel(Schema2TableModel *model)
{
    ui->table->setModel(model);
    ui->table->hideColumn(Schema2TableModel::col_name);
    ui->table->hideColumn(Schema2TableModel::col_type);
}
*/


static QList<int> selectedRows(QTableView* view) {
    QSet<int> res;
    auto indexes = view->selectionModel()->selectedIndexes();
    for(auto index: indexes) {
        res.insert(index.row());
    }
    QList<int> res_ = res.toList();
    qSort(res_.begin(), res_.end());
    return res_;
}

QStringList Schema2AlterView::selectedFields() const {
    QList<int> rows = selectedRows(ui->columns);
    QStringList res;
    for(int row: rows) {
        res.append(mModel->data(mModel->index(row, 0)).toString());
    }
    return res;
}

void Schema2AlterView::on_createRelation_clicked()
{
    QString parentTable = ui->parentTable->text();
    QString childTable = mModel->tableName();
    QStringList childColumns = selectedFields();
    if (childColumns.isEmpty()) {
        QMessageBox::information(this, "", "To create relation please select one or more columns");
        return;
    }
    emit createRelation(childTable, childColumns, parentTable);
}

void Schema2AlterView::createIndex(bool primary, bool unique) {
    QStringList columns = selectedFields();
    if (columns.isEmpty()) {
        QMessageBox::information(this, "", "To create index please select one or more columns");
        return;
    }
    QString indexName;
    if (primary) {
        indexName = QString("PK_%1").arg(mModel->tableName());
    } else {
        indexName = QString("IX_%1").arg(columns.join("_"));
    }
    mData->indexPulled(indexName, mModel->tableName(), columns, primary, unique, StatusNew);
}

void Schema2AlterView::on_createIndex_clicked()
{
    createIndex(false, false);
}


void Schema2AlterView::on_createUniqueIndex_clicked()
{
    createIndex(false, true);
}

void Schema2AlterView::on_createPrimaryKey_clicked()
{
    createIndex(true, false);
}



