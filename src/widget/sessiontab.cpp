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
#include "splitterutil.h"
#include "copyeventfilter.h"
#include "itemdelegate.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QStringListModel>
#include <QCompleter>
#include <QClipboard>
#include "tokens.h"
#include "highlighter.h"
#include "querymodelview.h"

#include "statview.h"

namespace {

void deleteLaterModel(QTableView* view) {
    QAbstractItemModel* model = view->model();
    view->setModel(0);
    if (model) {
        model->deleteLater();
    }
}

QStringList quote(const QStringList& items) {
    QStringList res;
    foreach(const QString& item, items) {
        res << "\"" + item + " \"";
    }
    return res;
}

QStringList unquote(const QStringList& items) {
    QStringList res;
    foreach(const QString& item, items) {
        QRegExp rx("^\\s*[\"](.*)[\"]\\s*$");
        if (item.indexOf(rx) > -1) {
            res << rx.cap(1).trimmed();
        } else {
            res << item;
        }
    }
    return res;
}

}

SessionTab::SessionTab(const QString &connectionName, const QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SessionTab), mConnectionName(connectionName), mName(name), mFirstQuery(true)
{
    ui->setupUi(this);
    cleanTabs();
    connect(ui->query,SIGNAL(submit()),this,SLOT(on_execute_clicked()));

    StatView* view = new StatView(ui->resultTabs);
    ui->resultTabs->insertTab(ui->resultTabs->count(),view,"stat");
    view->verticalHeader()->setDefaultSectionSize(40);
    view->horizontalHeader()->setStretchLastSection(true);
}

void SessionTab::on_resultTabs_currentChanged(int index) {
    if (QueryModelView* tab = qobject_cast<QueryModelView*>(ui->resultTabs->widget(index))) {
        tab->updateSplitter();
    }
}

StatView* SessionTab::statView() {
    return qobject_cast<StatView*>(ui->resultTabs->widget(ui->resultTabs->count()-1));
}

SessionTab::~SessionTab()
{
    qDebug() << "~SessionTab";
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

QueryModelView* SessionTab::tab(int index, bool* insert) {
    QueryModelView* tab = qobject_cast<QueryModelView*>(ui->resultTabs->widget(index));
    *insert = false;
    if (!tab) {
        *insert = true;
        if (!mTabs.isEmpty()) {
            tab = mTabs.takeFirst();
        } else {
            tab = new QueryModelView(ui->resultTabs);
        }
    }
    return tab;
}


void SessionTab::setResult(const QStringList& queries, const QStringList errors, const QList<QSqlQueryModel *> models, const QList<int> &perf, const QList<int> &rowsAffected)
{
    int i = 0;
    foreach(QSqlQueryModel * model, models) {
        if (model) {
            bool insert = false;
            QueryModelView* view = tab(i,&insert);
            view->setModel(model);
            QString title = QString("res %1").arg(i + 1);
            if (insert) {
                ui->resultTabs->insertTab(ui->resultTabs->count() - 1, view, title);
            }
            i++;
        }
    }

    QWidget* current = ui->resultTabs->currentWidget();

    bool removed = false;
    while(i < ui->resultTabs->count()) {
        QWidget* widget = ui->resultTabs->widget(i);
        QueryModelView* tab = qobject_cast<QueryModelView*>(widget);
        if (tab) {
            mTabs.append(tab);
            ui->resultTabs->removeTab(i);
            i--;
            removed = true;
            if (current == widget) {
                int index = ui->resultTabs->count() - 2;
                current = ui->resultTabs->widget(index > -1 ? index : 0);
            }
        }
        i++;
    }
    if (removed) {
        ui->resultTabs->setCurrentWidget(current);
    }

    StatView* view = statView();
    QueriesStatModel* model = new QueriesStatModel(queries, errors,perf,rowsAffected,view);
    deleteLaterModel(view);
    view->setModel(model);

    int columnWidth = (this->width() - 60 - view->horizontalHeader()->defaultSectionSize() * (model->hasErrors() ? 2 : 3)) / (model->hasErrors() ? 2 : 1);
    view->setColumnWidth(0,columnWidth);

    if (mFirstQuery) {
        SplitterUtil::setRatio(ui->splitter,1,5);
        ui->resultTabs->setCurrentIndex(0);
    }
    mFirstQuery = false;
}


void SessionTab::setTokens(const Tokens &tokens)
{
    ui->query->setTokens(tokens);
}


void SessionTab::cleanTabs()
{
    while(ui->resultTabs->count() > 0) {
        ui->resultTabs->widget(0)->deleteLater();
        ui->resultTabs->removeTab(0);
    }
    foreach (QueryModelView* view, mTabs) {
        view->deleteLater();
    }
    mTabs.clear();
}

void SessionTab::setQuery(const QString &query)
{
    ui->query->setPlainText(query);
}

void SessionTab::focusQuery()
{
    ui->query->setFocus();
}

QueryModelView* SessionTab::currentView() {
    return qobject_cast<QueryModelView*>(ui->resultTabs->currentWidget());
}

QSqlQueryModel *SessionTab::currentModel()
{
    QueryModelView* view = currentView();
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

    qDebug() << "dialog.output()" << dialog.output();

    QFile* file = 0;
    QTextStream* stream = 0;
    QString output;

    if (dialog.output() == OutputType::File) {
        QString filter;
        if (dialog.format() == DataFormat::Csv) {
            filter = "csv files (*.csv)";
        } else if (dialog.format() == DataFormat::Tsv) {
            filter = "tsv files (*.tsv)";
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

    QString error;

    DataStreamer::stream(*stream,model,dialog.format(),dialog.table(),
                         dialog.dataChecked(),dialog.keysChecked(),
                         DataFormat::ActionSave,locale(),error);

    if (!error.isEmpty()) {
        QMessageBox::critical(this,"Error",error);
        if (file) {
            file->deleteLater();
        }
        delete stream;
        return;
    }

    if (dialog.output() == OutputType::Session) {
        emit appendQuery(output);
    } else if (dialog.output() == OutputType::Clipboard) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(output);
    }

    if (file) {
        file->deleteLater();
    }

    delete stream;
}

void SessionTab::copySelected(bool asList)
{
    QSqlQueryModel* model = currentModel();
    if (!model) {
        return;
    }
    QString error;
    QItemSelection selection = currentView()->selectionModel()->selection();
    if (asList) {
        CopyEventFilter::copySelectedAsList(model, selection, locale(), error);
    } else {
        QString separator = "\t";
        DataFormat::Format format = DataFormat::Csv;
        CopyEventFilter::copySelected(model, selection, format, separator, locale(), error);
    }
    if (!error.isEmpty()) {
        QMessageBox::critical(this,"Error",error);
    }
}

void SessionTab::on_execute_clicked()
{
    emit query(ui->query->toPlainText());
}

void SessionTab::on_history_clicked()
{
    emit showQueryHistory();
}

void SessionTab::quoteQuery() {
    TextEdit* edit = ui->query;
    edit->setPlainText(quote(edit->toPlainText().split("\n")).join("\n"));
}

void SessionTab::unquoteQuery() {
    TextEdit* edit = ui->query;
    edit->setPlainText(unquote(edit->toPlainText().split("\n")).join("\n"));
}
