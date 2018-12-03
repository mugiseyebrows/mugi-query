#include "removedatabasedialog.h"
#include "ui_removedatabasedialog.h"

#include <QSqlDatabase>

RemoveDatabaseDialog::RemoveDatabaseDialog(const QString& name,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveDatabaseDialog),
    mName(name)
{
    ui->setupUi(this);
    ui->label->setText(QString("Are you sure you want to disconnect from %1?").arg(name));
}

RemoveDatabaseDialog::~RemoveDatabaseDialog()
{
    delete ui;
}

void RemoveDatabaseDialog::accept()
{
    QSqlDatabase db = QSqlDatabase::database(mName);
    db.close();
    QSqlDatabase::removeDatabase(mName);
    QDialog::accept();
}
