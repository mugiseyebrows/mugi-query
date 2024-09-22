#include "dataimportwidget2.h"
#include "ui_dataimportwidget2.h"

#include "dataimportmodel.h"
#include "schema2tablemodel.h"
#include "modelappender.h"
#include "copyeventfilter.h"
#include "clipboard.h"
#include "datetime.h"
#include "clipboardutil.h"
#include "richheaderview/richheaderview.h"
#include "richheaderview/richheaderdata.h"
#include "schema2data.h"
#include <QSqlDriver>
#include <QMessageBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QProgressDialog>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDatabase>
#include <QSqlQuery>


DataImportWidget2::DataImportWidget2(QWidget *parent) :
    mTable(nullptr),
    mAppender(new ModelAppender(this)),
    QWidget(parent),
    ui(new Ui::DataImportWidget2)
{
    ui->setupUi(this);


}

DataImportWidget2::~DataImportWidget2()
{
    delete ui;
}

static QMetaType::Type toMetaType(const QString& type) {

    QString type_ = type.toLower();

    QRegularExpression rxInt("int\\([0-9]+\\)");
    QRegularExpression rxVarchar("varchar\\([0-9]+\\)");
    if (rxInt.match(type_).hasMatch()) {
        return QMetaType::Int;
    }
    if (rxVarchar.match(type_).hasMatch()) {
        return QMetaType::QString;
    }
    if (type_ == "date") {
        return QMetaType::QDate;
    }
    if (type_ == "timestamp") {
        return QMetaType::QDateTime;
    }
    qDebug() << "toMetaType unknown type" << type_ << __FILE__ << __LINE__;
    return QMetaType::UnknownType;
}

QString metaTypeToString(QMetaType::Type type) {
    static QMap<QMetaType::Type, QString> types = {
        {QMetaType::Int, "Int"},
        {QMetaType::Double, "Double"},
        {QMetaType::QString, "QString"},
        {QMetaType::QDate, "QDate"},
        {QMetaType::QDateTime, "QDateTime"},
        {QMetaType::UnknownType, "UnknownType"},
        };
    return types.value(type, "?");
}

void DataImportWidget2::init(Schema2Data* data, Schema2TableModel *table)
{
    mData = data;
    mTable = table;
    setWindowTitle(table->tableName());
    initImportModel();
    initAppender();
    initCopyFilter();
    createHorizontalHeader();
}

void DataImportWidget2::initImportModel() {
    DataImportModel* model = new DataImportModel();
    mModel = model;
    Schema2TableModel *table = mTable;
    model->setColumnCount(mTable->rowCount());
    ui->table->setModel(model);
    QMap<int, QMetaType::Type> types;
    QMap<int, int> sizes;
    model->setHorizontalHeaderLabels(table->names());
    for(int row=0;row<table->rowCount();row++) {
        types[row] = toMetaType(table->type(row));
        /*qDebug() << "name" << table->name(row)
                 << "type" << table->type(row)
                 << "metaType" << metaTypeToString(toMetaType(table->type(row)));
        */
        sizes[row] = -1;
    }
    model->setTypes(types, sizes);
}

void DataImportWidget2::initAppender() {
    mAppender->setModel(mModel);
    mAppender->setActive(true);
}

void DataImportWidget2::initCopyFilter() {
    CopyEventFilter* filter = new CopyEventFilter();
    filter->setView(ui->table);
    connect(filter,SIGNAL(paste()),this,SLOT(onDataPaste()));
    connect(filter,SIGNAL(delete_()),filter,SLOT(onDeleteSelected()));
    connect(filter,SIGNAL(copy()),this,SLOT(onDataCopy()));
    connect(filter,&CopyEventFilter::cut,[=](){
        onDataCopy();
        filter->onDeleteSelected();
    });
    mFilter = filter;
}

void DataImportWidget2::createHorizontalHeader() {
    RichHeaderView* view = new RichHeaderView(Qt::Horizontal,ui->table);
    int fontHeight = view->fontMetrics().height();
    int twoLines = fontHeight * 3 / 2;
    RichHeaderData data = view->data();
    data.subsectionSizes({twoLines, twoLines});
    ui->table->setHorizontalHeader(view);
    mHeaderView = view;
    updateHorizontalHeader();
}

void DataImportWidget2::updateHorizontalHeader() {

    // qDebug() << "createHeaderViewWidgets" << mUpdatePreview;

    RichHeaderView* view = mHeaderView;
    QAbstractItemModel* model = mModel;
    if (!view || !model) {
        return;
    }
    RichHeaderData data = view->data();
    for(int c=0;c<model->columnCount();c++) {
        data.cell(0, c).text(mTable->name(c));
        data.cell(1, c).text(mTable->type(c)).textColor(Qt::gray);
    }
    view->update();
}


void DataImportWidget2::onDataCopy() {
    ClipboardUtil::copyTsv(ui->table);
}


static int countChar(const QString& s, QChar ch) {
    int count = 0;
    for(int i=0;i<s.size();i++) {
        if (s[i] == ch) {
            count += 1;
        }
    }
    return count;
}

static int closest(int target, const QList<int>& values) {
    int best = 0;
    int dbest = abs(values[best] - target);
    for(int i=1;i<values.size();i++) {
        int d = abs(values[i] - target);
        if (d < dbest) {
            dbest = d;
            best = i;
        }
    }
    return best;
}

static QAbstractItemModel* parseCsv(const QString& path, int expectedColumnCount, QStandardItemModel* model) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, "Error", QString("Cannot open %1").arg(path));
        return nullptr;
    }
    QTextStream stream(&file);

    QChar sep = '\0';

    //QStandardItemModel* model = new QStandardItemModel(10, expectedColumnCount);

    int row = 0;
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if (sep.isNull()) {
            int tabCount = countChar(line, '\t');
            int commaCount = countChar(line, ',');
            int semicolonCount = countChar(line, ';');
            int ix = closest(expectedColumnCount - 1, {tabCount, commaCount, semicolonCount});
            switch(ix) {
            case 0: sep = '\t'; break;
            case 1: sep = ','; break;
            case 2: sep = ';'; break;
            }
        }
        QStringList cols = line.split(sep);
        for(int column=0;column<cols.size();column++) {
            model->setData(model->index(row, column), cols[column]);
        }
        row += 1;
        if (row >= model->rowCount()) {
            model->setRowCount(model->rowCount() * 2);
        }
    }
    model->setRowCount(row - 1);
    return model;
}

void DataImportWidget2::on_openCsv_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, QString(), QString(), "Csv files (*.csv);; All files (*.*)");
    if (path.isEmpty()) {
        return;
    }

    mAppender->setActive(false);
    parseCsv(path, mTable->rowCount(), mModel);
    mAppender->setActive(true);

    /*QTableView* view = new QTableView();
    view->setModel(model);
    view->show();*/



}

void DataImportWidget2::on_alterTable_clicked()
{

}


QSqlRecord DataImportWidget2::record(QSqlDatabase db, const QString& tableName, int row, bool* ok) {
    QSqlRecord record = db.record(tableName);
    *ok = false;
    for(int column=0;column<mModel->columnCount();column++) {
        QVariant value = mModel->parsed(row, column);
        QString name = mTable->name(column);
        if (!value.isNull()) {
            record.setValue(name, value);
            *ok = true;
        }
    }
    return record;
}



void DataImportWidget2::on_execute_clicked()
{
    // todo check pending schema changes

    QSqlDatabase db = QSqlDatabase::database(mData->connectionName());
    QSqlQuery q(db);

    QSqlDriver* driver = db.driver();
    auto tableName = mTable->tableName();

    QStringList failedQueries;
    QStringList errors;
    int count = 0;

    QProgressDialog dialog(this);
    dialog.setMaximum(mModel->rowCount());
    dialog.show();

    bool ok;
    for(int row=0;row<mModel->rowCount();row++) {
        QSqlRecord record = this->record(db, tableName, row, &ok);
        if (ok) {
            QString query = driver->sqlStatement(QSqlDriver::InsertStatement, tableName, record, false);
            if (q.exec(query)) {
                count += 1;
            } else {
                failedQueries.append(query);
                errors.append(q.lastError().text());
            }
        }
        dialog.setValue(row);
        qApp->processEvents();
    }

    dialog.close();

    if (errors.size() > 0) {
        QStandardItemModel* errorModel = new QStandardItemModel(errors.size(), 2);
        for(int row=0;row<errors.size();row++) {
            errorModel->setData(errorModel->index(row, 0), errors[row]);
            errorModel->setData(errorModel->index(row, 1), failedQueries[row]);
        }
        QTableView* view = new QTableView();
        view->setModel(errorModel);
        view->horizontalHeader()->setStretchLastSection(true);
        view->show();
        view->setWindowTitle("Errors");
    } else {
        QMessageBox::information(this, "Success", QString("%1 rows inserted").arg(count));
    }


}

void DataImportWidget2::on_save_clicked()
{

}

static QModelIndex currentOrFirstIndex(QTableView* view) {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        index = view->model()->index(0,0);
    }
    return index;
}


void DataImportWidget2::onDataPaste() {

    QModelIndex topLeft = currentOrFirstIndex(ui->table);

    mAppender->setActive(false);
    QModelIndex bottomRight = Clipboard::pasteTsv(mModel,topLeft,true,false);
    mAppender->setActive(true);

    //createHeaderViewWidgets();

}

void DataImportWidget2::on_copy_clicked()
{
    onDataCopy();
}

void DataImportWidget2::on_paste_clicked()
{
    onDataPaste();
}


void DataImportWidget2::on_delete__clicked()
{
    mFilter->onDeleteSelected();
}

void DataImportWidget2::on_clear_clicked()
{
    auto ans = QMessageBox::question(this, "", "Clear all data?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ans != QMessageBox::Yes) {
        return;
    }
    mModel->setRowCount(0);
    mModel->setRowCount(10);
}

