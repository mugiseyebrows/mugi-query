#include "schema2relatedtableswidget.h"
#include "ui_schema2relatedtableswidget.h"

#include <QStringListModel>
#include "filterempty.h"
#include "schema2tablesmodel.h"
#include "schema2tablemodel.h"
#include "tolower.h"
#include "itemdelegatewithcompleter.h"
#include "schema2relationsmodel.h"
#include "modelappender.h"
#include "copyeventfilter.h"

Schema2RelatedTablesWidget::Schema2RelatedTablesWidget(Schema2TablesModel *tables, const QString &table, QWidget *parent) :
    QWidget(parent), mTables(tables),
    ui(new Ui::Schema2RelatedTablesWidget)
{
    ui->setupUi(this);

    QStringListModel* input = new QStringListModel();
    QStringListModel* output = new QStringListModel();

    QStringList names = toLower(mTables->tableNames());
    auto* delegate = new ItemDelegateWithCompleter(names, ui->input);
    ui->input->setItemDelegate(delegate);

    connect(input, &QAbstractItemModel::dataChanged, [=](){
        listRelated(input, output);
    });

    ui->input->setModel(input);
    ui->output->setModel(output);

    ModelAppender::attach(input);

    input->setStringList({table, ""});
    listRelated(input, output);

    CopyEventFilter::copyTsv(ui->input);
    CopyEventFilter::copyTsv(ui->output);

}

void Schema2RelatedTablesWidget::listRelated(QStringListModel* input, QStringListModel* output) {

    QStringList res;

    QStringList queue;
    //queue.append(mModel->tableName());

    QStringList inputTables = filterEmpty(input->stringList());

    for(const QString& table: inputTables) {
        if (mTables->contains(table)) {
            queue.append(mTables->table(table)->tableName());
        }
    }

    if (queue.isEmpty()) {
        return;
    }


    QStringList done;

    while (!queue.isEmpty()) {
        QString item = queue.takeFirst();
        if (done.contains(item)) {
            continue;
        }
        auto relations = mTables->table(item)->relations()->values();
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

    output->setStringList(res);

    /*QTableView* view = new QTableView();
        QStringListModel* model = new QStringListModel(res);
        view->setModel(model);
        showAndRaise(view);

        view->horizontalHeader()->setStretchLastSection(true);
        view->horizontalHeader()->setVisible(false);

        CopyEventFilter::copyTsv(view);*/

}


Schema2RelatedTablesWidget::~Schema2RelatedTablesWidget()
{
    delete ui;
}
