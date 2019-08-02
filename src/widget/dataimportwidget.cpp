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
#include <QCheckBox>
#include <QtGlobal>
#include <QSqlRecord>
#include <QSqlField>
#include "callonce.h"
#include "dataformat.h"
#include "datetime.h"

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

bool hasMatch(const QList<QRegularExpression>& exps, const QString text) {
    foreach(const QRegularExpression& exp, exps) {
        if (exp.match(text).hasMatch()) {
            return true;
        }
    }
    return false;
}

} // namespace

DataImportWidget::DataImportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataImportWidget),
    mUpdatePreview(new CallOnce("onUpdatePreview",0,this)),
    mSetModelTypes(new CallOnce("onSetTypes",0,this)),
    mSetColumnNamesAndTypes(new CallOnce("onSetColumnNamesAndTypes",0,this))
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
    connect(mSetModelTypes,SIGNAL(call()),this,SLOT(onModelSetTypes()));

    mConnectionName = connectionName;

    //connect(ui->existingTable,SIGNAL(currentIndexChanged(int)),this,SLOT(onExistingTableCurrentIndexChanged(int)));

    RichHeaderView* view = new RichHeaderView(Qt::Horizontal,ui->data);

    int fontHeight = view->fontMetrics().height();
    int twoLines = fontHeight * 2;

    RichHeaderData* data = view->data();
    data->subsectionSizes(Lit::il(twoLines,twoLines,twoLines));

    DataImportModel* model = new DataImportModel(data->sizeHint(),ui->data);
    model->setLocale(locale());

    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            mUpdatePreview,SLOT(onPost()));

    ui->data->setModel(model);
    ui->data->setHorizontalHeader(view);

    CopyEventFilter* filter = new CopyEventFilter();
    filter->setView(ui->data);

    connect(filter,SIGNAL(paste()),this,SLOT(onDataPaste()));
    connect(filter,SIGNAL(delete_()),filter,SLOT(onDeleteSelected()));
    connect(filter,SIGNAL(copy()),this,SLOT(onDataCopy()));

    on_abc_clicked();

    DataFormat::initComboBox(ui->format, true);

    connect(mSetColumnNamesAndTypes,SIGNAL(call()),this,SLOT(onSetColumnNamesAndTypes()));

    connect(ui->columns,SIGNAL(dataChanged(int,QModelIndex,QModelIndex)),
            mSetColumnNamesAndTypes,SLOT(onPost()));

    onUpdateTitle();
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
    mSetModelTypes->onPost();
}

QString DataImportWidget::connectionName() const
{
    return mConnectionName;
}

void DataImportWidget::on_existingTable_currentIndexChanged(int index) {

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

    //onColumnDataChanged(0,QModelIndex(),QModelIndex());
    mSetColumnNamesAndTypes->onPost();
    onUpdateTitle();
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

    QAbstractItemModel* model = dataModel();
    if (model->columnCount() < names.size()) {
        model->insertColumns(model->columnCount(), names.size() - model->columnCount());
    }

    createHeaderViewWidgets();

    for(int c=0;c<model->columnCount();c++) {
        QLineEdit* name = widgetName(c);
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

    QAbstractItemModel* model = dataModel();

    QStringList names = SqlDataTypes::names();
    QMap<QVariant::Type, QString> m = SqlDataTypes::mapFromVariant();

    for(int c=0;c<model->columnCount();c++) {
        QComboBox* type = widgetType(c);
        if (!type) {
            qDebug() << "!type" << __FILE__ << __LINE__;
            continue;
        }
        if (types.size() > c) {
            type->setCurrentIndex(names.indexOf(m[types[c]]));
        } else {
            type->setCurrentIndex(0);
        }
    }

}

QWidget* DataImportWidget::widget(int row, int column) {
    RichHeaderView* view = headerView();
    if (!view) {
        return 0;
    }
    RichHeaderData* data = view->data();
    return data->cell(row,column).widget();
}

QLineEdit* DataImportWidget::widgetName(int column) {
    return qobject_cast<QLineEdit*>(widget(WidgetName,column));
}

QComboBox* DataImportWidget::widgetType(int column) {
    return qobject_cast<QComboBox*>(widget(WidgetType,column));
}

QCheckBox* DataImportWidget::widgetPrimaryKey(int column) {
    return qobject_cast<QCheckBox*>(widget(WidgetPrimaryKey,column));
}

void DataImportWidget::namesTypesAndKeys(QStringList& names, QStringList& types, QList<bool>& primaryKeys) {

    RichHeaderView* view = headerView();
    QAbstractItemModel* model = dataModel();
    if (!view || !model) {
        qDebug() << "!view || !model" << __FILE__ << __LINE__;
        return;
    }
    for(int c=0;c<model->columnCount();c++) {
        QLineEdit* name = widgetName(c);
        QComboBox* type = widgetType(c);
        QCheckBox* primaryKey = widgetPrimaryKey(c);

        if (!name || !type || !primaryKey) {
            qDebug() << "!name || !type || !primaryKey" << __FILE__ << __LINE__;
            continue;
        }
        names << name->text();
        types << type->currentText();
        primaryKeys << primaryKey->isChecked();
    }
}



void DataImportWidget::createHeaderViewWidgets() {

    // qDebug() << "createHeaderViewWidgets" << mUpdatePreview;

    RichHeaderView* view = headerView();
    QAbstractItemModel* model = dataModel();
    if (!view || !model) {
        return;
    }

    bool newTable = ui->optionNewTable->isChecked();

    RichHeaderData* data = view->data();
    QWidget* viewport = view->viewport();
    for(int c=0;c<model->columnCount();c++) {

        QLineEdit* name = widgetName(c);
        if (!name) {
            name = new QLineEdit(viewport);
            data->cell(WidgetName,c).widget(name);

            connect(name, &QLineEdit::textChanged,[=](){
                onColumnNameChanged(c);
            });

        }

        QComboBox* types = widgetType(c);
        if (!types) {
            types = new QComboBox(viewport);

            types->addItems(SqlDataTypes::names());
            data->cell(WidgetType,c).widget(types);

            connect(types,qOverload<int>(&QComboBox::currentIndexChanged),[=](){
                onColumnTypeChanged(c);
            });

        }

        QCheckBox* primaryKey = widgetPrimaryKey(c);
        if (!primaryKey) {
            primaryKey = new QCheckBox("PK",viewport);
            data->cell(WidgetPrimaryKey,c).widget(primaryKey).padding(0,5);
            connect(primaryKey,qOverload<bool>(&QCheckBox::clicked),[=](){
                onColumnPrimaryKeyClicked(c);
            });
        }

        widgetName(c)->setReadOnly(!newTable);
        widgetType(c)->setEnabled(newTable);
        widgetPrimaryKey(c)->setEnabled(newTable);
    }
    view->update();
}



/*
void DataImportWidget::onColumnDataChanged(int,QModelIndex,QModelIndex) {
    qDebug() << "onColumnDataChanged";
}*/

void DataImportWidget::onSetColumnNamesAndTypes() {
    qDebug() << "onSetColumnNamesAndTypes";
    QStringList names;
    QList<QVariant::Type> types;
    ui->columns->checked(names,types);
    setColumnNames(names);
    setColumnTypes(types);
}

void DataImportWidget::onUpdateTokens(QString connectionName, Tokens tokens)
{
    if (connectionName != mConnectionName) {
        return;
    }
    update(tokens);
}

void DataImportWidget::onModelSetTypes() {

    DataImportModel* model = dataModel();
    if (!model) {
        return;
    }

    QMap<int,QVariant::Type> columnType;

    QStringList names;
    QStringList types;
    QList<bool> primaryKeys;
    namesTypesAndKeys(names,types,primaryKeys);

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
    mSetModelTypes->onPost();
}

void DataImportWidget::onColumnPrimaryKeyClicked(int) {
    mUpdatePreview->onPost();
}

void DataImportWidget::onColumnNameChanged(int) {
    // qDebug() << "onColumnNameChanged updatePreview" << column << mUpdatePreview;
    //mUpdatePreview->onPost();
    mSetModelTypes->onPost();
}

void DataImportWidget::newOrExistingTable() {
    bool newTable = ui->optionNewTable->isChecked();
    ui->existingTable->setEnabled(!newTable);
    ui->newTable->setEnabled(newTable);
    ui->columns->setVisible(!newTable);
    ui->format->setEnabled(!newTable);
    setSpacerEnabled(ui->verticalSpacer,newTable,ui->groupBox->layout());
    createHeaderViewWidgets();
    onUpdateTitle();
    mUpdatePreview->onPost();
}

void DataImportWidget::on_optionNewTable_clicked()
{
    newOrExistingTable();
}

void DataImportWidget::on_optionExistingTable_clicked()
{
    newOrExistingTable();
    //onColumnDataChanged(0,QModelIndex(),QModelIndex());
    mSetColumnNamesAndTypes->onPost();
    if (!ui->columns->data()->model()) {
        on_existingTable_currentIndexChanged(ui->existingTable->currentIndex());
    }
}

QVariant::Type DataImportWidget::guessType(QAbstractItemModel* model,
                                    const QModelIndex& topLeft,
                                    const QModelIndex& bottomRight) {

    int ints = 0;
    int dates = 0;
    int dateTimes = 0;
    int doubles = 0;
    int strings = 0;
    int times = 0;
    int total = 0;

    QRegularExpression rxInt("^[+-]?[0-9]+$");
    QRegularExpression rxDouble("^([+-]?(?:[0-9]+[.,]?|[0-9]*[.,][0-9]+))(?:[Ee][+-]?[0-9]+)?$");

    QRegularExpression rxTime("^(?:[01][0-9]|2[0-3]):(?:[0-5][0-9]):(?:[0-5][0-9])$");

    QList<QRegularExpression> rxDate = DateTime::dateRegularExpressions();
    QList<QRegularExpression> rxDateTime = DateTime::dateTimeRegularExpressions();

    QItemSelection selection(topLeft, bottomRight);
    QModelIndexList indexes = selection.indexes();
    foreach(const QModelIndex& index, indexes) {
        QString text = model->data(index).toString();
        if (text.isEmpty()) {
            continue;
        }
        if (rxInt.match(text).hasMatch()) {
            ints++;
        } else if (rxDouble.match(text).hasMatch()) {
            doubles++;
        } else if (rxTime.match(text).hasMatch()) {
            times++;
        } else if (hasMatch(rxDate,text)) {
            dates++;
        } else if (hasMatch(rxDateTime,text)) {
            dateTimes++;
        } else {
            strings++;
        }
        total++;
    }

    if (total < 1) {
        return QVariant::Invalid;
    }

    if (dates * 100 / total > 90) {
        return QVariant::Date;
    } else if (ints * 100 / total > 90) {
        return QVariant::Int;
    } else if (doubles * 100 / total > 90) {
        return QVariant::Double;
    } else if (times * 100 / total > 90) {
        return QVariant::Time;
    } else if (strings * 100 / total > 90) {
        return QVariant::String;
    } else if (dateTimes * 100 / total > 90) {
        return QVariant::DateTime;
    }
    return QVariant::Invalid;
}

void DataImportWidget::guessColumnType(int column) {
    QAbstractItemModel* model = dataModel();
    QMap<QVariant::Type, QString> m = SqlDataTypes::mapFromVariant();
    QStringList ts = SqlDataTypes::names();
    QVariant::Type type = guessType(model,model->index(0,column),model->index(model->rowCount()-1,column));
    if (type == QVariant::Invalid) {
        return;
    }
    QComboBox* types = widgetType(column);
    types->setCurrentIndex(ts.indexOf(m[type]));
}

void DataImportWidget::onDataPaste() {

    QAbstractItemModel* model = dataModel();
    QModelIndex topLeft = currentOrFirstIndex(ui->data);
    QModelIndex bottomRight = Clipboard::pasteTsv(model,topLeft,true,true);

    if (!ui->optionNewTable->isChecked()) {
        return;
    }

    for(int column = topLeft.column(); column <= bottomRight.column(); column++) {
        guessColumnType(column);
    }

    //onColumnTypeChanged(0);
    //mUpdatePreview->onPost();
}

void DataImportWidget::onDataCopy() {
    QString error;
    Clipboard::copySelected(dataModel(),ui->data->selectionModel()->selection(), DataFormat::Csv, "\t", locale(), error);
    Error::show(this,error);
}

QString DataImportWidget::queries(bool preview) {
    DataImportModel* model = dataModel();
    if (!model) {
        return QString();
    }



    QStringList names;
    QStringList types;
    QList<bool> primaryKeys;
    namesTypesAndKeys(names,types,primaryKeys);

    bool newTable = ui->optionNewTable->isChecked();

    QString table = tableName();

    QSqlDatabase db = QSqlDatabase::database(mConnectionName);

    QString createQuery = newTable ? DataStreamer::createTableStatement(db, table, names, types, primaryKeys, false) + ";\n" : QString();

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

QString DataImportWidget::tableName() {
    bool newTable = ui->optionNewTable->isChecked();
    QString table = newTable ? ui->newTable->text() : ui->existingTable->currentText();
    return table;
}

void DataImportWidget::onUpdateTitle()
{
    setWindowTitle(QString("%1 %2").arg(mConnectionName).arg(tableName()));
}

void DataImportWidget::on_newTable_textChanged(QString)
{
    if (!ui->optionNewTable->isChecked()) {
        return;
    }
    mUpdatePreview->onPost();
    onUpdateTitle();
}

void DataImportWidget::on_clearData_clicked()
{
    DataImportModel* model = dataModel();
    model->clear();
    model->setRowCount(5);
    if (ui->optionExistingTable->isChecked()) {
        //onColumnDataChanged(0,QModelIndex(),QModelIndex());
        mSetColumnNamesAndTypes->onPost();
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
    setColumnNames(alp.mid(0,dataModel()->columnCount()));
}

void DataImportWidget::on_format_currentIndexChanged(int)
{
    ui->columns->setFormat(DataFormat::value(ui->format));
    mSetColumnNamesAndTypes->onPost();
}

void DataImportWidget::on_guessTypes_clicked()
{
    int columnCount = dataModel()->columnCount();
    for(int column=0;column < columnCount; column++) {
        guessColumnType(column);
    }
}
