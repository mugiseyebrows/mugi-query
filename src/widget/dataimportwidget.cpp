#include "dataimportwidget.h"
#include "ui_dataimportwidget.h"

#include <QStandardItemModel>

#include "richheaderview/richheaderview.h"
#include "lit.h"
#include "model/dataimportmodel.h"
#include "model/checkablestringlistmodel.h"
#include "model/dataimportcolumnmodel.h"
#include "widget/selectcolumnslistwidget.h"
#include "copyeventfilter.h"
#include "clipboard.h"
#include <QTimer>
#include "splitterutil.h"
#include "datastreamer.h"
#include "sqldatatypes.h"
#include "error.h"
#include <QComboBox>
#include <QtGlobal>
#include <QSqlRecord>
#include <QSqlField>
#include "callonce.h"
#include "dataformat.h"

namespace  {

QModelIndex currentOrFirstIndex(QTableView* view) {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        index = view->model()->index(0,0);
    }
    return index;
}

void setSpacerEnabled(QSpacerItem* spacer, bool enabled, QLayout* layout) {
    if (enabled) {
        spacer->changeSize(1,1,QSizePolicy::Expanding,QSizePolicy::Expanding);
    } else {
        spacer->changeSize(0,0,QSizePolicy::Fixed,QSizePolicy::Fixed);
    }
    layout->invalidate();
}

QStringList join(const QList<QPair<QString,QString> > items, const QString& glue) {
    QStringList result;
    typedef QPair<QString,QString> T;
    foreach (const T& item, items) {
        result << item.first + glue + item.second;
    }
    return result;
}

void recordToNamesTypes(const QSqlRecord& record, QStringList& names, QList<QVariant::Type>& types) {
    for(int c=0;c<record.count();c++) {
        names << record.fieldName(c);
        types << record.field(c).type();
    }
    //qDebug() << "recordToNamesTypes";
}

}



DataImportWidget::DataImportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataImportWidget),
    mUpdatePreview(new CallOnce("onUpdatePreview",0,this)),
    mSetTypes(new CallOnce("onSetTypes",0,this))
{
    ui->setupUi(this);
}

DataImportWidget::~DataImportWidget()
{
    delete ui;
}

void DataImportWidget::init(const QString &connectionName)
{
    QTimer::singleShot(0,[=](){
        SplitterUtil::setRatio(ui->horizontalSplitter,1,2);
        SplitterUtil::setRatio(ui->verticalSplitter,4,1);
    });

    ui->columns->setLabelsMode(SelectColumnsWidget::LabelsModeShort);

    connect(mUpdatePreview,SIGNAL(call()),this,SLOT(onUpdatePreview()));
    connect(mSetTypes,SIGNAL(call()),this,SLOT(onSetTypes()));

    mConnectionName = connectionName;

    connect(ui->existingTable,SIGNAL(currentIndexChanged(int)),this,SLOT(onExistingTableCurrentIndexChanged(int)));

    RichHeaderView* view = new RichHeaderView(Qt::Horizontal,ui->data);
    RichHeaderData* data = view->data();
    data->subsectionSizes(Lit::il(30,30));

    DataImportModel* model = new DataImportModel(data->sizeHint(),ui->data);
    model->setLocale(locale());

    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            mUpdatePreview,SLOT(onPost()));

    ui->data->setModel(model);
    ui->data->setHorizontalHeader(view);

    CopyEventFilter* filter = new CopyEventFilter();
    filter->setView(ui->data);

    connect(filter,SIGNAL(paste()),this,SLOT(onDataPaste()));
    connect(filter,SIGNAL(delete_()),filter,SLOT(onDeleteSelected()));
    connect(filter,SIGNAL(copy()),this,SLOT(onDataCopy()));

    on_abc_clicked();

    connect(ui->format,qOverload<int>(QComboBox::currentIndexChanged),[=](int){
        ui->columns->setFormat(DataFormat::value(ui->format));
        mUpdatePreview->onPost();
    });

    DataFormat::initComboBox(ui->format, true);
}

QStringList comboBoxItems(QComboBox* comboBox) {
    QStringList items;
    for(int i=0;i<comboBox->count();i++) {
        items << comboBox->itemText(i);
    }
    return items;
}

void DataImportWidget::update(const Tokens& tokens) {
    mTokens = tokens;

    QStringList currentTables = comboBoxItems(ui->existingTable);
    QStringList newTables = mTokens.tables();

    bool firstRun = ui->existingTable->count() == 0;

    if (newTables != currentTables) {
        QString selected = ui->existingTable->currentText();
        ui->existingTable->clear();
        ui->existingTable->addItems(newTables);
        int index = selected.isEmpty() ? 0 : qMax(0,newTables.indexOf(selected));
        if (newTables.size() > 0) {
            ui->existingTable->setCurrentIndex(index);
        }
    }

    if (firstRun) {
        if (ui->optionNewTable->isChecked()) {
            on_optionNewTable_clicked();
        } else {
            on_optionExistingTable_clicked();
        }
    }

    ui->preview->setTokens(mTokens);
    mSetTypes->onPost();
}

QString DataImportWidget::connectionName() const
{
    return mConnectionName;
}

void DataImportWidget::onExistingTableCurrentIndexChanged(int index) {

    if (!ui->optionExistingTable->isChecked()) {
        return;
    }

    QString table = ui->existingTable->itemText(index);

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    QSqlRecord record = db.record(table);

    QStringList names;
    QList<QVariant::Type> types;
    recordToNamesTypes(record,names,types);

    ui->columns->setTable(table);
    ui->columns->setFields(names, types);

    ui->columns->data()->model()->setAllChecked();

    connect(ui->columns,SIGNAL(dataChanged(int,QModelIndex,QModelIndex)),
            this,SLOT(onColumnDataChanged(int,QModelIndex,QModelIndex)));

    onColumnDataChanged(0,QModelIndex(),QModelIndex());
}

/*
void DataImportWidget::onDataChanged(QModelIndex,QModelIndex,QVector<int>) {
    // qDebug() << "onDataChanged updatePreview";
    mUpdatePreview->onPost();
}*/

RichHeaderView* DataImportWidget::headerView() {
    return qobject_cast<RichHeaderView*>(ui->data->horizontalHeader());
}

DataImportModel* DataImportWidget::dataModel() {
    return qobject_cast<DataImportModel*>(ui->data->model());
}

void DataImportWidget::setColumnNames(const QStringList& names) {

    RichHeaderView* view = headerView();
    if (!view) {
        return;
    }

    QAbstractItemModel* dataModel = ui->data->model();
    if (dataModel->columnCount() < names.size()) {
        dataModel->insertColumns(dataModel->columnCount(), names.size() - dataModel->columnCount());
    }

    createHeaderViewWidgets();

    RichHeaderData* data = view->data();
    for(int c=0;c<dataModel->columnCount();c++) {
        QLineEdit* name = qobject_cast<QLineEdit*>(data->cell(WidgetName,c).widget());
        if (!name) {
            qDebug() << "!name" << __FILE__ << __LINE__;
            continue;
        }
        name->setText(names.value(c));
    }

    // qDebug() << "setColumnNames updatePreview";
}

void DataImportWidget::setColumnTypes(const QList<QVariant::Type>& types) {

    RichHeaderView* view = headerView();
    if (!view) {
        return;
    }

    QAbstractItemModel* dataModel = ui->data->model();

    QStringList names = SqlDataTypes::names();
    QMap<QVariant::Type, QString> m = SqlDataTypes::mapFromVariant();

    RichHeaderData* data = view->data();
    for(int c=0;c<dataModel->columnCount();c++) {
        QComboBox* type = qobject_cast<QComboBox*>(data->cell(WidgetType,c).widget());
        if (!type) {
            qDebug() << "!name" << __FILE__ << __LINE__;
            continue;
        }
        if (types.size() > c) {
            type->setCurrentIndex(names.indexOf(m[types[c]]));
        } else {
            type->setCurrentIndex(0);
        }
    }

}


void DataImportWidget::namesAndTypes(QStringList& names, QStringList& types) {

    RichHeaderView* view = headerView();
    QAbstractItemModel* model = ui->data->model();
    if (!view || !model) {
        return;
    }
    RichHeaderData* data = view->data();
    for(int c=0;c<model->columnCount();c++) {
        QLineEdit* name = qobject_cast<QLineEdit*>(data->cell(WidgetName,c).widget());
        QComboBox* type = qobject_cast<QComboBox*>(data->cell(WidgetType,c).widget());
        if (!name || !type) {
            qDebug() << "!name || !type" << __FILE__ << __LINE__;
            continue;
        }
        names << name->text();
        types << type->currentText();
    }
}



void DataImportWidget::createHeaderViewWidgets() {

    // qDebug() << "createHeaderViewWidgets" << mUpdatePreview;

    RichHeaderView* view = headerView();
    QAbstractItemModel* model = ui->data->model();
    if (!view || !model) {
        return;
    }

    bool newTable = ui->optionNewTable->isChecked();

    RichHeaderData* data = view->data();
    QWidget* viewport = view->viewport();
    for(int c=0;c<model->columnCount();c++) {

        QLineEdit* name = qobject_cast<QLineEdit*>(data->cell(WidgetName,c).widget());
        if (!name) {
            name = new QLineEdit(viewport);
            data->cell(WidgetName,c).widget(name);

            connect(name, &QLineEdit::textChanged,[=](){
                onColumnNameChanged(c);
            });

        }

        QComboBox* types = qobject_cast<QComboBox*>(data->cell(WidgetType,c).widget());
        if (!types) {
            types = new QComboBox(viewport);

            types->addItems(SqlDataTypes::names());
            data->cell(WidgetType,c).widget(types);

            connect(types,qOverload<int>(&QComboBox::currentIndexChanged),[=](){
                onColumnTypeChanged(c);
            });

        }

        data->cell(WidgetType,c).widget()->setEnabled(newTable);
        data->cell(WidgetName,c).widget()->setEnabled(newTable);
    }
    view->update();

}

void DataImportWidget::onColumnDataChanged(int,QModelIndex,QModelIndex) {
    qDebug() << "onColumnDataChanged";
    QStringList names;
    QList<QVariant::Type> types;
    ui->columns->checked(names,types);
    setColumnNames(names);
    setColumnTypes(types);
}

void DataImportWidget::onSetTypes() {

    DataImportModel* model = dataModel();
    if (!model) {
        return;
    }

    QMap<int,QVariant::Type> columnType;

    QStringList names;
    QStringList types;
    namesAndTypes(names,types);

    QMap<QString,QVariant::Type> m = SqlDataTypes::mapToVariant();

    for(int c=0;c<names.size();c++) {
        if (names[c].isEmpty()) {
            continue;
        }
        columnType[c] = m[types[c]];
    }

    model->setTypes(columnType);
    mUpdatePreview->onPost();
}

void DataImportWidget::onColumnTypeChanged(int) {
    mSetTypes->onPost();
}

void DataImportWidget::onColumnNameChanged(int column) {
    // qDebug() << "onColumnNameChanged updatePreview" << column << mUpdatePreview;
    mUpdatePreview->onPost();
}

void DataImportWidget::newOrExistingTable() {
    bool newTable = ui->optionNewTable->isChecked();
    ui->existingTable->setEnabled(!newTable);
    ui->newTable->setEnabled(newTable);
    ui->columns->setVisible(!newTable);
    setSpacerEnabled(ui->verticalSpacer,newTable,ui->groupBox->layout());
    createHeaderViewWidgets();
}

void DataImportWidget::on_optionNewTable_clicked()
{
    newOrExistingTable();
}

void DataImportWidget::on_optionExistingTable_clicked()
{
    newOrExistingTable();
    onColumnDataChanged(0,QModelIndex(),QModelIndex());
    if (!ui->columns->data()->model()) {
        onExistingTableCurrentIndexChanged(ui->existingTable->currentIndex());
    }
}

void DataImportWidget::onDataPaste() {
    Clipboard::pasteTsv(ui->data->model(),currentOrFirstIndex(ui->data),true,true);
    //onColumnTypeChanged(0);
    //mUpdatePreview->onPost();
}

void DataImportWidget::onDataCopy() {
    QString error;
    Clipboard::copySelected(ui->data->model(),ui->data->selectionModel()->selection(), DataFormat::Csv, "\t", locale(), error);
    Error::show(this,error);
}

QString DataImportWidget::queries(bool preview) {
    DataImportModel* model = this->dataModel();
    if (!model) {
        return QString();
    }

    bool newTable = ui->optionNewTable->isChecked();

    QStringList names;
    QStringList types;
    namesAndTypes(names,types);

    QString table = newTable ? ui->newTable->text() : ui->existingTable->currentText();

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QString createQuery = newTable ? DataStreamer::createTableStatement(db, table, names, types, false) + ";\n" : QString();

    QString error;
    bool hasMore;

    int rowCount = preview ? 5 : model->rowCount();

    DataFormat::Format format =
            newTable ?
                DataFormat::SqlInsert :
                DataFormat::value(ui->format);

    int dataColumns =
            format == DataFormat::SqlInsert ?
                names.size() :
                ui->columns->data()->model()->countChecked();

    QString insertOrUpdateQuery = DataStreamer::stream(format, db, model, rowCount, table, names, types, dataColumns, locale(), &hasMore, error);
    QString elipsis = (hasMore && preview) ? "..." : QString();

    return createQuery + insertOrUpdateQuery + elipsis;
}

void DataImportWidget::onUpdatePreview() {
    qDebug() << "onUpdatePreview";
    ui->preview->setText(queries(true));
}

void DataImportWidget::on_newTable_textChanged(QString)
{
    if (!ui->optionNewTable->isChecked()) {
        return;
    }
    mUpdatePreview->onPost();
}

void DataImportWidget::on_clearData_clicked()
{
    DataImportModel* model = dataModel();
    model->clear();
    model->setRowCount(5);
    if (ui->optionExistingTable->isChecked()) {
        onColumnDataChanged(0,QModelIndex(),QModelIndex());
    } else {
        model->setColumnCount(5);
        createHeaderViewWidgets();
    }
    mUpdatePreview->onPost();
}

void DataImportWidget::on_copyQuery_clicked()
{
    emit appendQuery(queries(false));
}

void DataImportWidget::on_abc_clicked()
{
    if (ui->optionExistingTable->isChecked()) {
        return;
    }
    QStringList alp = QString("abcdefghijklmnopqrstuvwxyz").split("", QString::SkipEmptyParts);
    setColumnNames(alp.mid(0,ui->data->model()->columnCount()));
}
