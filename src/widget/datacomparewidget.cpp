#include "datacomparewidget.h"
#include "ui_datacomparewidget.h"

#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlField>
#include "model/datacomparemodel.h"
#include "copyeventfilter.h"
#include "clipboardutil.h"
#include "itemdelegate.h"

static void recordToNamesTypes(const QSqlRecord& record, QStringList& names, QList<QMetaType::Type>& types) {
    for(int c=0;c<record.count();c++) {
        names << record.fieldName(c);
        types << (QMetaType::Type) record.field(c).metaType().id();
    }
}

DataCompareWidget::DataCompareWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataCompareWidget)
{
    ui->setupUi(this);

    DataCompareModel* model = new DataCompareModel();
    ui->view->setModel(model);

    //ui->columns->setLabelText("Key columns", "");

    connect(ui->columns,SIGNAL(dataChanged(int,QModelIndex,QModelIndex)),this,SLOT(onColumnsChanged()));

    connect(ui->modeShort,SIGNAL(clicked(bool)),this,SLOT(onModeChanged()));
    connect(ui->modeFull,SIGNAL(clicked(bool)),this,SLOT(onModeChanged()));
    connect(ui->modeBefore,SIGNAL(clicked(bool)),this,SLOT(onModeChanged()));
    connect(ui->modeAfter,SIGNAL(clicked(bool)),this,SLOT(onModeChanged()));

    CopyEventFilter* filter = new CopyEventFilter();
    filter->setView(ui->view);

    connect(filter,&CopyEventFilter::copy,[=](){
        ClipboardUtil::copyTsv(ui->view);
    });

    ItemDelegate* delegate = new ItemDelegate();
    ui->view->setItemDelegate(delegate);

    onModeChanged();
}

void DataCompareWidget::onModeChanged() {
    DataCompareModel* model = qobject_cast<DataCompareModel*>(ui->view->model());
    if (!model) {
        return;
    }
    QList<DataCompareModel::Mode> modes = {
        DataCompareModel::Short,
        DataCompareModel::Full,
        DataCompareModel::Before,
        DataCompareModel::After,
    };
    QList<QRadioButton*> buttons = {
        ui->modeShort,
        ui->modeFull,
        ui->modeBefore,
        ui->modeAfter
    };
    for(int i=0;i<modes.size();i++) {
        if (buttons[i]->isChecked()) {
            model->setMode(modes[i]);
            return;
        }
    }

}

void DataCompareWidget::onColumnsChanged() {

    DataCompareModel* model = qobject_cast<DataCompareModel*>(ui->view->model());
    if (!model) {
        return;
    }

    QList<bool> checkList = ui->columns->dataCheckList();
    QList<int> columns;
    for(int column=0;column<checkList.size();column++) {
        if (checkList[column]) {
            columns.append(column);
        }
    }

    model->setKeyColumns(columns);
}

void DataCompareWidget::setModels(QSqlQueryModel* model1, QSqlQueryModel* model2) {

    QStringList names;
    QList<QMetaType::Type> types;
    recordToNamesTypes(model1->record(0), names, types);
    ui->columns->setFields(names, types);
    ui->columns->setFormat(DataFormat::SqlInsert);

    DataCompareModel* model = qobject_cast<DataCompareModel*>(ui->view->model());
    if (!model) {
        return;
    }
    model->setModels(model1, model2);
}

DataCompareWidget::~DataCompareWidget()
{
    delete ui;
}
