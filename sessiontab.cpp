#include "sessiontab.h"
#include "ui_sessiontab.h"

#include <QSqlQueryModel>
#include <QTableView>
#include <QStandardItemModel>
#include <QDebug>
#include "queriesstatmodel.h"
#include "savedatadialog.h"
#include <QFileDialog>
#include "datastreamer.h"
#include "setsplitersizesratio.h"
#include "copyeventfilter.h"
#include "itemdelegate.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QStringListModel>
#include <QCompleter>
#include "tokens.h"
#include "highlighter.h"

SessionTab::SessionTab(const QString &connectionName, const QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SessionTab), mConnectionName(connectionName), mName(name), mFirstQuery(true)
{
    ui->setupUi(this);
    cleanTabs();
    connect(ui->query,SIGNAL(submit()),this,SLOT(on_execute_clicked()));
}


SessionTab::~SessionTab()
{
    delete ui;
}

QString SessionTab::connectionName() const
{
    return mConnectionName;
}

QString SessionTab::name() const
{
    return mName;
}

void resizeColumnsToContents(QTableView* view, int maxWidth) {
    QAbstractItemModel* model = view->model();
    if (!model) {
        return;
    }
    view->resizeColumnsToContents();
    for(int c=0; c<model->columnCount(); c++) {
        if (view->columnWidth(c) > maxWidth)
            view->setColumnWidth(c,maxWidth);
    }
}

void SessionTab::setResult(const QStringList& queries, const QStringList errors, const QList<QSqlQueryModel *> models, const QList<int> &perf, const QList<int> &rowsAffected)
{
    cleanTabs();

    int i = 0;

    foreach(QSqlQueryModel * model, models) {
        if (model) {
            QTableView* view = new QTableView();
            view->setModel(model);
            QString title = QString("res %1").arg(++i);
            ui->resultTabs->insertTab(ui->resultTabs->count(),view,title);
            resizeColumnsToContents(view,this->width()/2);
            CopyEventFilter* filter = new CopyEventFilter(view);
            filter->setView(view);

            ItemDelegate* delegate = new ItemDelegate(view);
            view->setItemDelegate(delegate);

        }
    }

    QTableView* view = new QTableView();
    view->verticalHeader()->setDefaultSectionSize(40);
    view->horizontalHeader()->setStretchLastSection(true);

    QueriesStatModel* model = new QueriesStatModel(queries, errors,perf,rowsAffected,view);
    view->setModel(model);

    int columnWidth = (this->width() - 60 - view->horizontalHeader()->defaultSectionSize() * (model->hasErrors() ? 2 : 3)) / (model->hasErrors() ? 2 : 1);

    view->setColumnWidth(0,columnWidth);

    QString title = "stat";
    ui->resultTabs->insertTab(ui->resultTabs->count(),view,title);

    if (mFirstQuery) {
        setSpliterSizesRatio(ui->splitter,1,5);
    }
    mFirstQuery = false;
}


void SessionTab::setTokens(const Tokens &tokens)
{
    QCompleter* completer = new QCompleter();
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);

    QStringListModel* stringListModel = new QStringListModel(tokens.autocompletion(),completer);
    completer->setModel(stringListModel);

    ui->query->setCompleter(completer);

    Highlighter* highlighter = new Highlighter(tokens,0);
    ui->query->setHighlighter(highlighter);
}


void SessionTab::cleanTabs()
{
    while(ui->resultTabs->count() > 0) {
        ui->resultTabs->widget(0)->deleteLater();
        ui->resultTabs->removeTab(0);
    }
}

void SessionTab::setQuery(const QString &query)
{
    ui->query->setPlainText(query);
}

void SessionTab::focusQuery()
{
    ui->query->setFocus();
}

QTableView* SessionTab::currentView() {
    return qobject_cast<QTableView*>(ui->resultTabs->currentWidget());
}

QSqlQueryModel *SessionTab::currentModel()
{
    QTableView* view = currentView();
    if (!view) {
        return 0;
    }
    return qobject_cast<QSqlQueryModel*>(view->model());
}

void SessionTab::saveData()
{
    QSqlQueryModel* model = currentModel();
    if (!model) {
        return;
    }

    SaveDataDialog dialog(model,this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QFile* file = 0;
    QTextStream* stream = 0;
    QString output;

    if (dialog.output() == OutputType::File) {
        QString filter;
        if (dialog.format() == DataFormat::Csv) {
            filter = "csv files (*.csv)";
        } else {
            filter = "sql files (*.sql)";
        }
        QString name = QFileDialog::getSaveFileName(this,QString(),QString(),filter);
        if (name.isEmpty()) {
            return;
        }
        file = new QFile(name);
        if (!file->open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this,"Error",QString("Can not open file %1").arg(name));
            file->deleteLater();
            return;
        }
        stream = new QTextStream(file);
    } else {
        stream = new QTextStream(&output,QIODevice::WriteOnly);
    }

    DataStreamer::stream(*stream,model,dialog.format(),dialog.table(),dialog.dataChecked(),dialog.keysChecked());

    if (dialog.output() == OutputType::NewSession) {
        emit addSessionWithQuery(output);
    } else if (dialog.output() == OutputType::CurrentSession) {
        setQuery(output);
    }

    if (file) {
        file->deleteLater();
    }

    delete stream;
}

void SessionTab::copySelected()
{
    QSqlQueryModel* model = currentModel();
    if (!model) {
        return;
    }
    QItemSelection selection = currentView()->selectionModel()->selection();
    CopyEventFilter::copySelected(model,selection);
}

void SessionTab::on_execute_clicked()
{
    emit query(ui->query->toPlainText());
}

void SessionTab::on_history_clicked()
{
    emit showQueryHistory();
}

void SessionTab::on_save_clicked()
{
    saveData();
}
