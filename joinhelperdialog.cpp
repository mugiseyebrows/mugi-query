#include "joinhelperdialog.h"
#include "ui_joinhelperdialog.h"

#include <QDir>
#include "settings.h"
#include <QSqlDatabase>
#include <QStandardItemModel>
#include <QTextStream>
#include <QTextCodec>
#include <QMessageBox>
#include "modelappender.h"
#include "itemdelegatewithcompleter.h"
#include "checkablestringlistmodel.h"
#include "rowvaluegetter.h"
#include <vector>
#include <QDebug>
#include "relation.h"
#include "relations.h"

#include "relationsmodel.h"

JoinHelperDialog::JoinHelperDialog(const QString& connectionName,
                                   const Tokens& tokens, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinHelperDialog),
    mConnectionName(connectionName)
{
    ui->setupUi(this);
    ui->query->setTokens(tokens);

    RelationsModel* model = new RelationsModel(this);

    QString path = filePath();
    model->load(path);
    model->insertRow(model->rowCount());
    ui->relations->setModel(model);

    ItemDelegateWithCompleter* itemDelegate = new ItemDelegateWithCompleter(tokens.fields(true),this);
    ui->relations->setItemDelegate(itemDelegate);

    ModelAppender* appender = new ModelAppender(this);
    appender->setModel(model);

    QStringListModel* allTables = new QStringListModel(tokens.tables(),this);
    QStandardItemModel* selectedTables = new QStandardItemModel(0,1,this);

    ui->allTables->setModel(allTables);
    ui->selectedTables->setModel(selectedTables);

    /*CheckableStringListModel* tablesModel = new CheckableStringListModel(tokens.tables(),this);
    ui->tables->setModel(tablesModel);*/

}

JoinHelperDialog::~JoinHelperDialog()
{
    delete ui;
}

QString JoinHelperDialog::filePath() const
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QStringList props;
    props << "relations" << db.driverName() << db.hostName()
          << db.userName() << db.databaseName();

    QString name = props.join(" ")
            .replace(QRegExp("[^a-z0-9._ -]",Qt::CaseInsensitive)," ")
            .replace(QRegExp("[ ]+")," ") + ".txt";

    return QDir(Settings::instance()->dir()).filePath(name);
}



void JoinHelperDialog::findPath()
{
    Relations relations(ui->relations->model());

    QAbstractItemModel* model = ui->selectedTables->model();
    QStringList tables;
    for(int row=0;row<model->rowCount();row++) {
        tables << model->data(model->index(row,0)).toString();
    }
    Relations::PathList path = relations.findPath(tables);
    if (path.isEmpty()) {
        ui->query->setText("no path found");
    } else {
        ui->query->setText(relations.expression(path,true,true));
    }

}

void JoinHelperDialog::accept()
{
    qobject_cast<RelationsModel*>(ui->relations->model())->save(filePath());
    QDialog::accept();
}

void JoinHelperDialog::on_add_clicked()
{
    QModelIndexList indexes = ui->allTables->selectionModel()->selectedIndexes();
    QAbstractItemModel* model = ui->selectedTables->model();
    foreach(const QModelIndex& index, indexes) {
        int row = model->rowCount();
        model->insertRow(row);
        model->setData(model->index(row,0),index.data());
    }

    if (model->rowCount() > 1) {
        findPath();
    }

}

void JoinHelperDialog::on_remove_clicked()
{
    QModelIndexList indexes = ui->selectedTables->selectionModel()->selectedIndexes();
    qSort(indexes);
    QAbstractItemModel* model = ui->selectedTables->model();
    for(QModelIndexList::reverse_iterator it = indexes.rbegin(); it != indexes.rend(); it++) {
        model->removeRow(it->row());
    }

    if (model->rowCount() > 1) {
        findPath();
    } else {
        ui->query->setText("");
    }
}
