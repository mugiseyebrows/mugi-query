#include "removedatabasedialog.h"
#include "ui_removedatabasedialog.h"

#include <QSqlDatabase>

RemoveDatabaseDialog::RemoveDatabaseDialog(const QString& connectionName,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveDatabaseDialog),
    mConnectionName(connectionName)
{
    ui->setupUi(this);
    ui->label->setText(QString("Are you sure you want to disconnect from %1?").arg(connectionName));
}

RemoveDatabaseDialog::~RemoveDatabaseDialog()
{
    delete ui;
}

void RemoveDatabaseDialog::accept()
{
    QSqlDatabase db = QSqlDatabase::database(mConnectionName);
    db.close();
    QSqlDatabase::removeDatabase(mConnectionName);
    QDialog::accept();
}
