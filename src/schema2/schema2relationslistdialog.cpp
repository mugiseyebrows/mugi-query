#include "schema2relationslistdialog.h"
#include "ui_schema2relationslistdialog.h"

#include "schema2relationmodel.h"
#include "schema2relationslistmodel.h"
#include "schema2relationsfiltermodel.h"
#include "schema2tablemodel.h"

#include <QResizeEvent>

Schema2RelationsListDialog::Schema2RelationsListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Schema2RelationsListDialog)
{
    ui->setupUi(this);
}

#include <QCompleter>
#include <QStringListModel>

static QCompleter* createCompleter(QAbstractItemModel* model,
                            QCompleter::CompletionMode completionMode,
                            Qt::MatchFlags filterMode,
                            QCompleter::ModelSorting sorting,
                            Qt::CaseSensitivity caseSensitivity,
                            QObject* parent
                            ) {

    QCompleter* completer = new QCompleter(parent);
    completer->setModel(model);
    /*completer->setCompletionMode(completionMode);
    completer->setFilterMode(filterMode);
    completer->setModelSorting(sorting);*/
    completer->setCaseSensitivity(caseSensitivity);
    return completer;
}

void Schema2RelationsListDialog::init(const StringHash<Schema2TableModel *> &tableModels,
        const StringListHash<Schema2RelationModel*>& relationModels) {

    mModel = new Schema2RelationsListModel(relationModels, this);
    mProxyModel = new Schema2RelationsFilterModel();
    mProxyModel->setSourceModel(mModel);
    ui->table->setModel(mProxyModel);

    QStringListModel* tables = new QStringListModel(tableModels.keys());

    ui->childTable->setCompleter(createCompleter(tables, QCompleter::InlineCompletion, Qt::MatchContains, QCompleter::UnsortedModel, Qt::CaseInsensitive, ui->childTable));
    ui->parentTable->setCompleter(createCompleter(tables, QCompleter::InlineCompletion, Qt::MatchContains, QCompleter::UnsortedModel, Qt::CaseInsensitive, ui->parentTable));
}

Schema2RelationsListDialog::~Schema2RelationsListDialog()
{
    delete ui;
}

QString Schema2RelationsListDialog::childTable() const
{
    QModelIndex current = ui->table->currentIndex();
    if (!current.isValid()) {
        return ui->childTable->text();
    }
    return mProxyModel->data(mProxyModel->index(current.row(), 0)).toString();
}

QString Schema2RelationsListDialog::parentTable() const
{
    QModelIndex current = ui->table->currentIndex();
    if (!current.isValid()) {
        return ui->parentTable->text();
    }
    return mProxyModel->data(mProxyModel->index(current.row(), 1)).toString();
}

bool Schema2RelationsListDialog::remove() const
{
    return false;
}

void Schema2RelationsListDialog::on_table_doubleClicked(const QModelIndex &index)
{

}

void Schema2RelationsListDialog::on_childTable_textChanged(QString table)
{
    mProxyModel->setChildTable(table);
}

void Schema2RelationsListDialog::on_parentTable_textChanged(QString table)
{
    mProxyModel->setParentTable(table);
}

#include <QDebug>

void Schema2RelationsListDialog::resizeEvent(QResizeEvent *event)
{

    /*qDebug() << "table" << ui->table->geometry()
             << "viewport" << ui->table->viewport()->geometry();*/

    int size = ui->table->viewport()->width() / 2;
    size = qMax(100, size);

    ui->table->setColumnWidth(0, size);
    return QDialog::resizeEvent(event);
}
