#include "schema2alterview.h"
#include "ui_schema2alterview.h"

#include "schema2tablemodel.h"
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
#include "tablestretcher.h"
#include "drivernames.h"
#include "clipboardutil.h"
#include "copyeventfilter.h"
#include "schema2parentrelationsmodel.h"
#include <QStringListModel>
#include "showandraise.h"
#include "schema2relatedtableswidget.h"
#include "ones.h"
#include <algorithm>

// todo push / pull schema for one table

// todo forbid dropping index if used in relation (odbc)

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
    ui->columns->hideColumn(Schema2TableModel::col_name_prev);
    ui->columns->hideColumn(Schema2TableModel::col_type_prev);
    ui->columns->hideColumn(Schema2TableModel::col_notnull_prev);
    ui->columns->hideColumn(Schema2TableModel::col_default_prev);
    ui->columns->hideColumn(Schema2TableModel::col_autoincrement_prev);

    TableButtons* buttons = new TableButtons();

    const int button_remove = 0;
    const int button_insert = 1;

    buttons->button(button_remove).inside().text("-").size(40, 40).offset(40, 0);
    buttons->button(button_insert).between().text("+").size(40, 40);

    bool canInsert = mData->driverName() != DRIVER_SQLITE;

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
    ui->columns->setItemDelegateForColumn(Schema2TableModel::col_type, delegate);


    ui->childTable->setText(model->tableName());

    setCompleter(ui->parentTable, mTables->tableNames());

    CopyEventFilter::copyTsv(ui->columns);

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
            if (index < 0) {
                return;
            }
            mData->editRelationDialog(mModel->relations()->at(index), this);
        } else if (id == button_remove) {
            if (index < 0) {
                return;
            }
            auto* relation = mModel->relations()->at(index);
            mData->dropRelationDialog(relation, this);
        }
    });

    TableStretcher::stretch(ui->relations, ones(6), 80);

    CopyEventFilter::copyTsv(ui->relations);
}

void Schema2AlterView::initParentRelations()
{
    ui->parentRelations->setModel(mModel->parentRelations());

    const int button_edit = 0;
    const int button_remove = 1;

    TableButtons* buttons = new TableButtons();
    buttons->button(button_edit).inside().text("mod").size(40, 40);
    buttons->button(button_remove).inside().text("-").size(40, 40).offset(40, 0);

    buttons->setView(ui->parentRelations);
    connect(buttons, &TableButtons::clicked, [=](int id, int index){
        if (id == button_edit) {
            if (index < 0) {
                return;
            }
            mData->editRelationDialog(mModel->parentRelations()->at(index), this);
        } else if (id == button_remove) {
            if (index < 0) {
                return;
            }
            auto* relation = mModel->parentRelations()->at(index);
            mData->dropRelationDialog(relation, this);
        }
    });

    TableStretcher::stretch(ui->parentRelations, ones(6), 80);

    CopyEventFilter::copyTsv(ui->parentRelations);
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
            if (index < 0) {
                return;
            }
            mModel->indexes()->removeAt(index);
        }
    });


    TableStretcher::stretch(ui->indexes, ones(4), 40);

    CopyEventFilter::copyTsv(ui->indexes);
}

void Schema2AlterView::init(Schema2Data *data, Schema2TablesModel* tableModels,
                            Schema2TableModel *model, const QStringList& types) {
    mData = data;
    mTables = tableModels;
    mModel = model;
    mTypes = types;
    initColumns();
    initRelations();
    initParentRelations();
    initIndexes();
    setWindowTitle(mModel->tableName());
}

/*
void Schema2AlterView::setModel(Schema2TableModel *model)
{
    ui->table->setModel(model);
    ui->table->hideColumn(Schema2TableModel::col_name);
    ui->table->hideColumn(Schema2TableModel::col_type);
}
*/


template <typename T>
static QList<T> toList(const QSet<T>& qlist)
{
    return QList<T> (qlist.constBegin(), qlist.constEnd());
}



static QList<int> selectedRows(QTableView* view) {
    QSet<int> res;
    auto indexes = view->selectionModel()->selectedIndexes();
    for(auto index: indexes) {
        res.insert(index.row());
    }
    QList<int> res_ = toList(res);
    std::sort(res_.begin(), res_.end());
    return res_;
}

QStringList Schema2AlterView::selectedFields() const {
    QList<int> rows = selectedRows(ui->columns);
    QStringList res;
    for(int row: rows) {
        res.append(mModel->name(row));
    }
    return res;
}

void Schema2AlterView::on_createRelation_clicked()
{
    QString parentTable = ui->parentTable->text();
    //QString childTable = mModel->tableName();
    QStringList childColumns = selectedFields();
    if (childColumns.isEmpty()) {
        QMessageBox::information(this, "", "To create relation please select one or more columns");
        return;
    }
    //emit createRelation(childTable, childColumns, parentTable);
    mData->createRelationDialog(mModel, childColumns, parentTable);
}

void Schema2AlterView::createIndex(bool primary, bool unique) {
    QStringList columns = selectedFields();
    if (columns.isEmpty()) {
        QMessageBox::information(this, "", "To create index please select one or more columns");
        return;
    }
    QString indexName;
    if (primary) {
        indexName = QString("PK_%1").arg(mModel->tableName()).toLower();
    } else {
        indexName = QString("IX_%1").arg(columns.join("_")).toLower();
    }

    QString driverName = mData->driverName();
    if (driverName == DRIVER_MYSQL || driverName == DRIVER_MARIADB) {
        if (primary) {
            indexName = "PRIMARY";
        }
    }

    mTables->table(mModel->tableName())->insertIndex(indexName, columns, primary, unique, StatusNew);
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



void Schema2AlterView::on_listRelatedTables_clicked()
{

    Schema2RelatedTablesWidget* widget = new Schema2RelatedTablesWidget(mTables, mModel->tableName());
    showAndRaise(widget);


#if 0
    QStringList res;

    QStringList queue;
    queue.append(mModel->tableName());

    QStringList done;

    while (!queue.isEmpty()) {
        QString item = queue.takeFirst();
        if (done.contains(item)) {
            continue;
        }
        auto relations = mData->tables()->table(item)->relations()->values();
        for(auto* relation: relations) {
            QString parentTable = relation->parentTable();
            if (!res.contains(parentTable)) {
                res.append(parentTable);
            }
            if (!done.contains(parentTable)) {
                queue.append(parentTable);
            }
        }
        if (!res.contains(item)) {
            res.append(item);
        }
        done.append(item);
    }

    QTableView* view = new QTableView();
    QStringListModel* model = new QStringListModel(res);
    view->setModel(model);
    showAndRaise(view);

    view->horizontalHeader()->setStretchLastSection(true);
    view->horizontalHeader()->setVisible(false);

    CopyEventFilter::copyTsv(view);

#endif

}

