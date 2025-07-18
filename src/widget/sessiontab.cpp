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
#include <QTemporaryFile>
#include "tokens.h"
#include "highlighter.h"
#include "querymodelview.h"

#include "statview.h"
#include "clipboard.h"
#include <QSharedPointer>
#include "error.h"
#include "copyeventfilter.h"
#include "cornerbutton.h"
#include <QMenu>
#include "copytoclipboard.h"
#include "checkablestringlistmodel.h"
#include "dictkeydialog.h"
#include <QSqlRecord>
#include "fieldnames.h"

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
        auto trimmed = item.trimmed();
        if (trimmed.startsWith("\"") && trimmed.endsWith("\"")) {
            trimmed = trimmed.mid(1, trimmed.size() - 2);
            res.append(trimmed);
        } else {
            res.append(item);
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

    CopyEventFilter* filter = new CopyEventFilter(view);
    filter->setView(view);
    connect(filter, &CopyEventFilter::copy, [=](){


        QString error;
        const QItemSelection selection = view->selectionModel()->selection();
        Clipboard::copySelected(view->model(), selection, DataFormat::Csv, "\t", true, locale(), error);
        Error::show(this,error);
#if 0
        auto* model = view->model();
        auto selection = view->selectionModel()->selection();
        auto format = ui->format->format();

        copyToClipboard(model, selection, format);
#endif

    });


#if 0

    CornerButton* button = new CornerButton("size");
    button->setWidget(ui->resultTabs);
    connect(button->button(), &QPushButton::clicked, [=](){

        QMenu menu(ui->resultTabs);

        QAction* colsFit = menu.addAction("Fit Columns");
        QAction* colsS = menu.addAction("S Columns");
        QAction* colsM = menu.addAction("M Columns");
        QAction* colsL = menu.addAction("L Columns");
        QAction* colsXL = menu.addAction("XL Columns");

        QAction* rowsFit = menu.addAction("Fit Rows");
        QAction* rowsS = menu.addAction("S Rows");
        QAction* rowsM = menu.addAction("M Rows");
        QAction* rowsL = menu.addAction("L Rows");
        QAction* rowsXL = menu.addAction("XL Rows");

        QMap<QAction*, QueryModelView::Size> colsMap = {
            {colsFit, QueryModelView::SizeFit},
            {colsS, QueryModelView::SizeS},
            {colsM, QueryModelView::SizeM},
            {colsL, QueryModelView::SizeL},
            {colsXL, QueryModelView::SizeXL},
        };

        QMap<QAction*, QueryModelView::Size> rowsMap = {
            {rowsFit, QueryModelView::SizeFit},
            {rowsS, QueryModelView::SizeS},
            {rowsM, QueryModelView::SizeM},
            {rowsL, QueryModelView::SizeL},
            {rowsXL, QueryModelView::SizeXL}
        };

        QAction* res = menu.exec(button->button()->mapToGlobal(QPoint(0,0)));

        auto* view = currentView();

        if (colsMap.contains(res)) {
            auto size = colsMap[res];
            view->setColumnsWidth(size);
        }
        if (rowsMap.contains(res)) {
            auto size = rowsMap[res];
            view->setRowsHeight(size);
        }

    });
#endif


    connect(ui->format, &CopyFormatWidget::keyClicked, [=](){
        QSqlQueryModel* model = currentModel();
        QStringList names = fieldNames(model->record(0));
        auto* listModel = new CheckableStringListModel(names);
        DictKeyDialog dialog{listModel, this};
        if (dialog.exec() != QDialog::Accepted) {
            return;
        }
        ui->format->setKey(dialog.key());
    });


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
            connect(tab, &QueryModelView::copySelected, [=](){

                auto* model = currentModel();
                auto selection = currentView()->selectionModel()->selection();
                auto format = ui->format->format();

                QSqlDatabase db = QSqlDatabase::database(mConnectionName);
                QSqlDriver* driver = db.driver();

                copyToClipboard(model, selection, format, driver);
            });
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
        SplitterUtil::setRatio(ui->splitter,{1,5});
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

QString SessionTab::query() const
{
    return ui->query->toPlainText();
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

void SessionTab::fetchAll() {
    QSqlQueryModel* model = currentModel();
    if (!model) {
        return;
    }
    while (model->canFetchMore()) {
        model->fetchMore();
    }
}

void SessionTab::viewAsHex()
{
    auto* view = currentView();
    view->viewAsHex();
}

void SessionTab::viewAsBin()
{
    auto* view = currentView();
    view->viewAsBin();
}

void SessionTab::viewAsString() {
    auto* view = currentView();
    view->viewAsString();
}

void SessionTab::saveData()
{
    QSqlQueryModel* model = currentModel();
    if (!model) {
        return;
    }

    fetchAll();

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    SaveDataDialog dialog(mConnectionName,model,ui->query->tokens(),this);
    dialog.init();
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    dialog.save();

    //qDebug() << "dialog.output()" << dialog.output();

    //QSharedPointer<QFile> file;
    //QSharedPointer<QTextStream> stream;
    //QString output;

    QTemporaryFile tempFile(QDir(QDir::tempPath()).filePath("XXXXXX.tmp"));


    QString filePath;
    if (dialog.output() != OutputType::File) {
        tempFile.open();
        filePath = tempFile.fileName();
        tempFile.close();
    } else {
        filePath = dialog.filePath();
    }

    QFileInfo info(filePath);
    QDir dir = info.dir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.absolutePath())) {
            QString msg = QString("Can not create directory %1").arg(QDir::toNativeSeparators(dir.absolutePath()));
            QMessageBox::critical(this, "Error", msg);
            return;
        }
    }


    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly)) {
        QString msg = QString("Can not open file %1").arg(filePath);
        QMessageBox::critical(this, "Error", msg);
        return;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

#if 0
    if (dialog.output() == OutputType::File) {

        QString filePath = dialog.filePath();
        if (filePath.isEmpty()) {
            qDebug() << __FILE__ << __LINE__;
            return;
        }
        file = QSharedPointer<QFile>(new QFile(filePath));
        if (!file->open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this,"Error",QString("Can not open file %1").arg(filePath));
            return;
        }
        stream = QSharedPointer<QTextStream>(new QTextStream(file.get()));
    } else {
        stream = QSharedPointer<QTextStream>(new QTextStream(&output,QIODevice::WriteOnly));
    }

    if (!stream.isNull()) {
        stream->setEncoding(QStringConverter::Utf8);
    }
#endif

    QString error;
    bool hasMore;

    DataStreamer::stream(db,stream,model,dialog.format(),dialog.table(),
                         dialog.dataChecked(),dialog.keysChecked(),
                         DataFormat::ActionSave,false,&hasMore,locale(),error);

    if (!error.isEmpty()) {
        Error::show(this,error);
        return;
    }

    stream.flush();
    file.close();

    QString output;
    if (dialog.output() != OutputType::File) {
        if (!file.open(QIODevice::ReadOnly)) {
            QString msg = QString("Can not open file %1").arg(filePath);
            QMessageBox::critical(this, "Error", msg);
            return;
        }
        output = QString::fromUtf8(file.readAll());
    }

    if (dialog.output() == OutputType::Session) {
        emit appendQuery(output);
    } else if (dialog.output() == OutputType::Clipboard) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(output);
    }


}

void SessionTab::copySelected(CopyFormat fmt)
{
    QSqlQueryModel* model = currentModel();
    if (!model) {
        return;
    }
    QString error;
    QItemSelection selection = currentView()->selectionModel()->selection();
    if (fmt == CopyFormat::List) {
        Clipboard::copySelectedAsList(model, selection);
    } else if (fmt == CopyFormat::Table) {
        QString separator = "\t";
        DataFormat::Format format = DataFormat::Csv;
        Clipboard::copySelected(model, selection, format, separator, true, locale(), error);
    } else if (fmt == CopyFormat::Condition) {
        Clipboard::copySelectedAsCondition(model, selection);
    } else if (fmt == CopyFormat::Names) {
        Clipboard::copySelectedNames(model, selection);
    } else if (fmt == CopyFormat::Insert) {
        Clipboard::copySelectedAsInsert(model, selection);
    }
    if (!error.isEmpty()) {
        QMessageBox::critical(this,"Error",error);
    }
}

void SessionTab::on_execute_clicked()
{
    emit query(query());
}

void SessionTab::on_history_clicked()
{
    emit showQueryHistory();
}

void SessionTab::quoteQuery() {
    TextEdit* edit = ui->query;
    edit->setPlainText(quote(query().split("\n")).join("\n"));
}

void SessionTab::unquoteQuery() {
    TextEdit* edit = ui->query;
    edit->setPlainText(unquote(query().split("\n")).join("\n"));
}

QueryModelView *SessionTab::tab(int index)
{
    return qobject_cast<QueryModelView*>(ui->resultTabs->widget(index));
}

