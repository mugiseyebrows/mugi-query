#include "createtabledialog.h"
#include "ui_createtabledialog.h"
#include "schema2data.h"
#include "drivernames.h"
#include "schema2tablesmodel.h"
#include <QSqlDatabase>
#include "sdata.h"

CreateTableDialog::CreateTableDialog(Schema2Data* data, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateTableDialog)
{
    ui->setupUi(this);

    auto* tables = data->tables();
    auto schemaNames = tables->schemaNames();
    ui->schema->addItems(schemaNames);

    if (data->driverName() == DRIVER_MYSQL || data->driverName() == DRIVER_MARIADB) {
        auto db = data->database();
        ui->schema->setCurrentText(db.databaseName());
    }


}

CreateTableDialog::~CreateTableDialog()
{
    delete ui;
}

SName CreateTableDialog::name() const
{
    return SName(ui->schema->currentText(), ui->tableName->text());
}
