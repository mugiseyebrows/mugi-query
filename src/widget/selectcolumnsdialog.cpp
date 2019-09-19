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

void SelectColumnsDialog::init(const QString &table, const QStringList &fields, const QList<QVariant::Type> &types)
{
    ui->columns->setFields(fields, types);
    ui->columns->setTable(table);
    ui->columns->setFormat(DataFormat::SqlUpdate);
}

QStringList SelectColumnsDialog::dataChecked() const
{
    QStringList fields;
    QList<QVariant::Type> types;
    ui->columns->dataChecked(fields, types);
    return fields;
}

QStringList SelectColumnsDialog::keysChecked() const
{
    QStringList fields;
    QList<QVariant::Type> types;
    ui->columns->keysChecked(fields, types);
    return fields;
}
