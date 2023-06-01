#include "datatruncatedialog.h"
#include "ui_datatruncatedialog.h"

DataTruncateDialog::DataTruncateDialog(QSqlDatabase db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataTruncateDialog)
{
    ui->setupUi(this);

    auto tables = db.tables();
    ui->tables->append(tables);

}

DataTruncateDialog::~DataTruncateDialog()
{
    delete ui;
}

QStringList DataTruncateDialog::tables() const
{
    return ui->tables->checked();
}
