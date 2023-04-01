#include "comparetableitemwidget.h"
#include "ui_comparetableitemwidget.h"

#include <QSqlDatabase>
#include <QCompleter>
#include "tolower.h"
#include <QStringListModel>

CompareTableItemWidget::CompareTableItemWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CompareTableItemWidget)
{
    ui->setupUi(this);
}

CompareTableItemWidget::~CompareTableItemWidget()
{
    delete ui;
}

void CompareTableItemWidget::init(const QStringList &connectionNames)
{
    ui->database->addItems(connectionNames);
}

QString CompareTableItemWidget::database() const
{
    return ui->database->currentText();
}

QString CompareTableItemWidget::table() const
{
    return ui->table->text();
}



void CompareTableItemWidget::on_database_currentIndexChanged(int index)
{
    QString connectionName = ui->database->currentText();
    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QCompleter* completer = new QCompleter();
    QStringListModel* model = new QStringListModel(toLower(db.tables()));
    completer->setModel(model);
    ui->table->setCompleter(completer);
}

