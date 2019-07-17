#include "dataimportwidget.h"
#include "ui_dataimportwidget.h"

#include <QStandardItemModel>

#include "richheaderview/richheaderview.h"
#include "lit.h"
#include "model/dataimportmodel.h"
#include "model/checkablestringlistmodel.h"
#include "copyeventfilter.h"
#include "clipboard.h"
#include <QTimer>
#include "splitterutil.h"
#include "zipunzip.h"
#include "datastreamer.h"
#include "sqldatatypes.h"
#include <QComboBox>
#include <QtGlobal>

namespace  {

QModelIndex currentOrFirstIndex(QTableView* view) {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        index = view->model()->index(0,0);
    }
    return index;
}

void setSpacerEnabled(QSpacerItem* spacer, bool enabled) {
    if (enabled) {
        spacer->changeSize(1,1,QSizePolicy::Expanding,QSizePolicy::Expanding);
    } else {
        spacer->changeSize(0,0,QSizePolicy::Fixed,QSizePolicy::Fixed);
    }
}


QStringList join(const QList<QPair<QString,QString> > items, const QString& glue) {
    QStringList result;
    typedef QPair<QString,QString> T;
    foreach (const T& item, items) {
        result << item.first + glue + item.second;
    }
    return result;
}

}

DataImportWidget::DataImportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataImportWidget)
{
    ui->setupUi(this);

    QTimer::singleShot(0,this,SLOT(onTimeout()));
}

DataImportWidget::~DataImportWidget()
{
    delete ui;
}

void DataImportWidget::onTimeout() {
    SplitterUtil::setRatio(ui->horizontalSplitter,1,4);
    SplitterUtil::setRatio(ui->verticalSplitter,4,1);
}

void DataImportWidget::init(const QString &connectionName)
{
    mConnectionName = connectionName;

    connect(ui->existingTable,SIGNAL(currentIndexChanged(int)),this,SLOT(onExistingTableCurrentIndexChanged(int)));

    RichHeaderView* view = new RichHeaderView(Qt::Horizontal,ui->data);
    RichHeaderData* data = view->data();
    data->subsectionSizes(Lit::il(30,30));

    DataImportModel* model = new DataImportModel(data->sizeHint(),ui->data);
    model->setLocale(locale());

    ui->data->setModel(model);
    ui->data->setHorizontalHeader(view);

    //QStringList alp = QString("abcdefghijklmnopqrstuvwxyz").split("", QString::SkipEmptyParts);
    //setColumnNames(alp.mid(0,model->columnCount()));

    CopyEventFilter* filter = new CopyEventFilter();
    filter->setView(ui->data);

    connect(filter,SIGNAL(paste()),this,SLOT(onDataPaste()));
}

void DataImportWidget::update(const Tokens& tokens) {
    mTokens = tokens;

    ui->existingTable->clear();
    ui->existingTable->addItems(mTokens.tables());

    if (ui->optionNewTable->isChecked()) {
        on_optionNewTable_clicked();
    } else {
        on_optionExistingTable_clicked();
    }

    ui->preview->setTokens(mTokens);
}

QString DataImportWidget::connectionName() const
{
    return mConnectionName;
}

void DataImportWidget::onExistingTableCurrentIndexChanged(int index) {
    QString table = ui->existingTable->itemText(index);
    QStringList fields = mTokens.fields(table, false);
    CheckableStringListModel* model = new CheckableStringListModel(fields,this);
    model->setAllChecked();
    ui->columns->setModel(model);
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this,SLOT(onColumnsDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    onColumnsDataChanged(QModelIndex(),QModelIndex(),QVector<int>());
}

RichHeaderView* DataImportWidget::headerView() {
    return qobject_cast<RichHeaderView*>(ui->data->horizontalHeader());
}

CheckableStringListModel* DataImportWidget::columnsModel() {
    return qobject_cast<CheckableStringListModel*>(ui->columns->model());
}

DataImportModel* DataImportWidget::dataModel() {
    return qobject_cast<DataImportModel*>(ui->data->model());
}

bool DataImportWidget::hasAnyData() {
    return false;
}



void DataImportWidget::setColumnNames(const QStringList& names) {

    if (hasAnyData()) {
        return;
    }
    QAbstractItemModel* dataModel = ui->data->model();
    if (dataModel->columnCount() < names.size()) {
        dataModel->insertColumns(dataModel->columnCount(), names.size() - dataModel->columnCount());
    }

    createHeaderViewWidgets();

    RichHeaderView* view = headerView();
    if (!view) {
        return;
    }
    RichHeaderData* data = view->data();
    for(int c=0;c<dataModel->columnCount();c++) {
        QLineEdit* name = qobject_cast<QLineEdit*>(data->cell(0,c).widget());
        if (!name) {
            qDebug() << "!name" << __FILE__ << __LINE__;
            continue;
        }
        name->setText(names.value(c));
    }

}

QList<QPair<QString,QString> > DataImportWidget::namesAndTypes() {

    QList<QPair<QString,QString> > result;
    RichHeaderView* view = headerView();
    QAbstractItemModel* model = ui->data->model();
    if (!view || !model) {
        return result;
    }
    RichHeaderData* data = view->data();
    for(int c=0;c<model->columnCount();c++) {
        QLineEdit* name = qobject_cast<QLineEdit*>(data->cell(0,c).widget());
        QComboBox* type = qobject_cast<QComboBox*>(data->cell(1,c).widget());
        if (!name || !type) {
            qDebug() << "!name || !type" << __FILE__ << __LINE__;
            continue;
        }
        result << QPair<QString,QString>(name->text(),type->currentText());
    }
    return result;
}



void DataImportWidget::createHeaderViewWidgets() {
    RichHeaderView* view = headerView();
    QAbstractItemModel* model = ui->data->model();
    if (!view || !model) {
        return;
    }

    bool newTable = ui->optionNewTable->isChecked();

    RichHeaderData* data = view->data();
    QWidget* viewport = view->viewport();
    for(int c=0;c<model->columnCount();c++) {

        QLineEdit* name = qobject_cast<QLineEdit*>(data->cell(0,c).widget());
        if (!name) {
            name = new QLineEdit(viewport);
            data->cell(0,c).widget(name);

            connect(name, &QLineEdit::textChanged,[=](){
                onColumnNameChanged(c);
            });

        }

        QComboBox* types = qobject_cast<QComboBox*>(data->cell(1,c).widget());
        if (!types) {
            types = new QComboBox(viewport);

            types->addItems(SqlDataTypes::names());
            data->cell(1,c).widget(types);

            connect(types,qOverload<int>(&QComboBox::currentIndexChanged),[=](){
                onColumnTypeChanged(c);
            });

        }

        data->cell(0,c).show();
        data->cell(1,c).setVisible(newTable);
    }
    view->update();
    //view->showWidgets();
}

void DataImportWidget::onColumnsDataChanged(QModelIndex,QModelIndex,QVector<int>) {
    CheckableStringListModel* model = columnsModel();
    setColumnNames(model->checked());
    updatePreview();
}

void DataImportWidget::onColumnTypeChanged(int column) {

    DataImportModel* model = dataModel();
    if (!model) {
        return;
    }

    QMap<int,QVariant::Type> columnType;

    QList<QPair<QString,QString> > namesAndTypes = this->namesAndTypes();
    QStringList names = unzipFirsts(namesAndTypes);
    QStringList types = unzipSeconds(namesAndTypes);

    QMap<QString,QVariant::Type> m = SqlDataTypes::mapToVariant();

    for(int c=0;c<names.size();c++) {
        if (names[c].isEmpty()) {
            continue;
        }
        columnType[c] = m[types[c]];
    }

    model->setTypes(columnType);

    updatePreview();
}

void DataImportWidget::onColumnNameChanged(int column) {
    updatePreview();
}

void DataImportWidget::newOrExistingTable() {
    bool newTable = ui->optionNewTable->isChecked();
    ui->existingTable->setEnabled(!newTable);
    ui->newTable->setEnabled(newTable);

    ui->columns->setVisible(!newTable);
    ui->allColumns->setVisible(!newTable);
    ui->noneColumns->setVisible(!newTable);

    setSpacerEnabled(ui->verticalSpacer,newTable);
    ui->verticalLayout->update();
}

void DataImportWidget::on_optionNewTable_clicked()
{
    newOrExistingTable();
    QStringList alp = QString("abcdefghijklmnopqrstuvwxyz").split("", QString::SkipEmptyParts);
    setColumnNames(alp.mid(0,ui->data->model()->columnCount()));
    updatePreview();
}

void DataImportWidget::on_optionExistingTable_clicked()
{
    newOrExistingTable();
    onColumnsDataChanged(QModelIndex(),QModelIndex(),QVector<int>());
    updatePreview();
}

void DataImportWidget::on_allColumns_clicked()
{
    CheckableStringListModel* model = columnsModel();
    if (!model) {
        return;
    }
    model->setAllChecked();
}

void DataImportWidget::on_noneColumns_clicked()
{
    CheckableStringListModel* model = columnsModel();
    if (!model) {
        return;
    }
    model->setAllUnchecked();
}

void DataImportWidget::onDataPaste() {
    Clipboard::pasteTsv(ui->data->model(),currentOrFirstIndex(ui->data),true,true);
    onColumnTypeChanged(0);
    updatePreview();
}

void DataImportWidget::updatePreview() {

    DataImportModel* model = this->dataModel();
    if (!model) {
        return;
    }

    bool newTable = ui->optionNewTable->isChecked();

    auto namesAndTypes = this->namesAndTypes();

    QStringList names = unzipFirsts(namesAndTypes);
    QStringList types = unzipSeconds(namesAndTypes);

    QString table = newTable ? ui->newTable->text() : ui->existingTable->currentText();

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QString createQuery = newTable ? DataStreamer::createTableStatement(db, table, names, types, false) + ";\n" : QString();

    QString error;
    QString insertQuery = DataStreamer::stream(db, model, 5,table,names,types,locale(),error);

    ui->preview->setText(createQuery + insertQuery);
}

void DataImportWidget::on_newTable_textChanged(const QString &arg1)
{
    if (ui->optionNewTable->isChecked()) {
        updatePreview();
    }
}
