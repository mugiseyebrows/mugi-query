#include "schema2relationdialog2.h"
#include "ui_schema2relationdialog2.h"

#include "schema2relation.h"
#include <QStandardItemModel>
#include "tablebuttons/tablebuttons.h"
#include "schema2tablemodel.h"
#include "itemdelegatewithcompleter.h"
#include "tablebuttons.h"
#include "modelcolumn.h"


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
                                  const QString& relationName,
                                  const QStringList& childColumns,
                                  const QStringList& parentColumns)
{
    mChildTable = childTable;
    mParentTable = parentTable;

    ui->childTable->setText(childTable->tableName());

    ui->parentTable->setText(parentTable->tableName());

    ui->constraintName->setText(relationName);

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
            model->removeRow(index);
        } else if (id == button_insert) {
            model->insertRow(index);
        }
    });

}

QString Schema2RelationDialog2::relationName() const
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

