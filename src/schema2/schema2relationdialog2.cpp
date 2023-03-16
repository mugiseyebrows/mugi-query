#include "schema2relationdialog2.h"
#include "ui_schema2relationdialog2.h"

#include "schema2relation.h"
#include <QStandardItemModel>
#include "tablebuttons/tablebuttons.h"
#include "schema2tablemodel.h"
#include "itemdelegatewithcompleter.h"
#include "tablebuttons.h"

static void setModelColumn(QAbstractItemModel* model, int column, const QStringList& values) {
    for(int row=0;row<values.size();row++) {
        model->setData(model->index(row, column), values[row]);
    }
}

static QStringList modelColumn(QAbstractItemModel* model, int column) {
    QStringList res;
    for(int row=0;row<model->rowCount();row++) {
        QString value = model->data(model->index(row, column)).toString();
        res.append(value);
    }
    return res;
}


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

void Schema2RelationDialog2::init(Schema2TableModel *childTable,
                                  Schema2TableModel *parentTable,
                                  const QString& constraintName,
                                  const QStringList& childColumns,
                                  const QStringList& parentColumns)
{
    mChildTable = childTable;
    mParentTable = parentTable;

    ui->childTable->setText(childTable->tableName());

    ui->parentTable->setText(parentTable->tableName());

    ui->constraintName->setText(constraintName);

    QStandardItemModel* model = new QStandardItemModel(parentColumns.size(), 2);

    setModelColumn(model, 0, childColumns);
    setModelColumn(model, 1, parentColumns);

    ItemDelegateWithCompleter* childDelegate =
            new ItemDelegateWithCompleter(childTable->newNames(), ui->columns);
    ItemDelegateWithCompleter* parentDelegate =
            new ItemDelegateWithCompleter(parentTable->newNames(), ui->columns);

    ui->columns->setItemDelegateForColumn(0, childDelegate);
    ui->columns->setItemDelegateForColumn(1, parentDelegate);

    ui->columns->setModel(model);

    TableButtons* buttons = new TableButtons();

    const int button_remove = 0;
    const int button_insert = 1;

    buttons->button(button_remove).inside().text("-").size(40, 40);
    buttons->button(button_insert).between().text("+").size(40, 40).offset(40, 0);

    buttons->setView(ui->columns);
    connect(buttons, &TableButtons::clicked, [=](int id, int index){
        if (id == button_remove) {
            if (index < 0) {
                return;
            }
        } else if (id == button_insert) {

        }
    });

}

QString Schema2RelationDialog2::constraintName() const
{
    return ui->constraintName->text();
}

QStringList Schema2RelationDialog2::childColumns() const
{
    return modelColumn(ui->columns->model(), 0);
}

QStringList Schema2RelationDialog2::parentColumns() const
{
    return modelColumn(ui->columns->model(), 1);
}

bool Schema2RelationDialog2::constrained() const
{
    return true;
}

