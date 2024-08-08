#include "selectcolumnsdialog.h"
#include "ui_selectcolumnsdialog.h"

SelectColumnsDialog::SelectColumnsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectColumnsDialog)
{
    ui->setupUi(this);
}

SelectColumnsDialog::~SelectColumnsDialog()
{
    delete ui;
}

void SelectColumnsDialog::initUpdate(const QString &table, const QStringList &fields, const QList<QMetaType::Type> &types)
{
    ui->columns->setFields(fields, types);
    ui->columns->setTable(table);
    ui->columns->setFormat(DataFormat::SqlUpdate);
}

void SelectColumnsDialog::initInsert(const QString &table, const QStringList &fields, const QList<QMetaType::Type> &types) {
    ui->columns->setFields(fields, types);
    ui->columns->setTable(table);
    ui->columns->setFormat(DataFormat::SqlInsert);
}

QStringList SelectColumnsDialog::dataChecked() const
{
    QStringList fields;
    QList<QMetaType::Type> types;
    ui->columns->dataChecked(fields, types);
    return fields;
}

QStringList SelectColumnsDialog::keysChecked() const
{
    QStringList fields;
    QList<QMetaType::Type> types;
    ui->columns->keysChecked(fields, types);
    return fields;
}

QStringList SelectColumnsDialog::checked() const {
    QStringList fields;
    QList<QMetaType::Type> types;
    ui->columns->checked(fields, types);
    return fields;
}
