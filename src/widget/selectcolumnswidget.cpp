#include "selectcolumnswidget.h"
#include "ui_selectcolumnswidget.h"

#include "model/dataimportcolumnmodel.h"

SelectColumnsWidget::SelectColumnsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SelectColumnsWidget), mFormat(DataFormat::SqlInsert)
{
    ui->setupUi(this);
    connect(ui->data,&SelectColumnsListWidget::dataChanged,[=](QModelIndex index1,QModelIndex index2){
        emit dataChanged(0,index1,index2);
    });
    connect(ui->keys,&SelectColumnsListWidget::dataChanged,[=](QModelIndex index1,QModelIndex index2){
        emit dataChanged(1,index1,index2);
    });
}

SelectColumnsWidget::~SelectColumnsWidget()
{
    delete ui;
}

void SelectColumnsWidget::setFields(const QStringList& fields, const QList<QVariant::Type>& types) {
    ui->data->setFields(fields, types);
    ui->keys->setFields(fields, types);
}

QList<bool> SelectColumnsWidget::keysCheckList() const {
    return ui->keys->checkList();
}

QList<bool> SelectColumnsWidget::dataCheckList() const {
    return ui->data->checkList();
}


void SelectColumnsWidget::checked(QStringList &fields, QList<QVariant::Type> &types)
{
    if (!ui->data->model()) {
        return;
    }

    ui->data->model()->checked(fields, types);
    if (mFormat == DataFormat::SqlUpdate) {
        ui->keys->model()->checked(fields, types);
    }
}

void SelectColumnsWidget::dataChecked(QStringList &fields, QList<QVariant::Type> &types)
{
    ui->data->model()->checked(fields, types);
}

void SelectColumnsWidget::keysChecked(QStringList &fields, QList<QVariant::Type> &types)
{
    ui->keys->model()->checked(fields, types);
}

void SelectColumnsWidget::setLabelsMode(SelectColumnsWidget::LabelsMode mode)
{
    mLabelsMode = mode;
}

bool SelectColumnsWidget::hasAnyChecked() const
{
    if (mFormat == DataFormat::SqlUpdate) {
        return ui->data->hasAnyChecked() && ui->keys->hasAnyChecked();
    }
    return ui->data->hasAnyChecked();
}

void SelectColumnsWidget::setTable(const QString &table)
{
    mTable = table;
    setFormat(mFormat);
}

void SelectColumnsWidget::setFormat(DataFormat::Format f)
{
    mFormat = f;
    QString dataLabel;
    QString keysLabel;
    if (f == DataFormat::SqlInsert) {
        dataLabel = mLabelsMode == LabelsModeLong ? QString("INSERT INTO %1 (...)").arg(mTable) : "Data";
        keysLabel = QString();
    } else if (f == DataFormat::SqlUpdate) {
        dataLabel = mLabelsMode == LabelsModeLong ? QString("UPDATE %1 SET ...").arg(mTable) : "Data";
        keysLabel = mLabelsMode == LabelsModeLong ? "WHERE ..." : "Keys";
    } else if (f == DataFormat::Csv || f == DataFormat::Tsv) {
        dataLabel = QString();
        keysLabel = QString();
    }
    ui->data->setLabelText(dataLabel);
    ui->keys->setLabelText(keysLabel);
    ui->keys->setVisible(f == DataFormat::SqlUpdate);
}

void SelectColumnsWidget::setLabelText(const QString& dataLabel, const QString& keysLabel) {
    ui->data->setLabelText(dataLabel);
    ui->keys->setLabelText(keysLabel);
}

SelectColumnsListWidget *SelectColumnsWidget::data() const
{
    return ui->data;
}

SelectColumnsListWidget *SelectColumnsWidget::keys() const
{
    return ui->keys;
}
