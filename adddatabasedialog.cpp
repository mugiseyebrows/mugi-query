#include "adddatabasedialog.h"
#include "ui_adddatabasedialog.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>

AddDatabaseDialog::AddDatabaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDatabaseDialog)
{
    ui->setupUi(this);
    ui->driver->addItems(QSqlDatabase::drivers());
}

AddDatabaseDialog::~AddDatabaseDialog()
{
    delete ui;
}

QString AddDatabaseDialog::connectionName() {
    return ui->connectionName->text();
}

void AddDatabaseDialog::accept()
{
    QString connectionName = ui->connectionName->text();

    if (connectionName.isEmpty()) {
        QMessageBox::critical(this,"Error",QString("Specify name"));
        return;
    }

    if (QSqlDatabase::connectionNames().contains(connectionName)) {
        QMessageBox::critical(this,"Error",QString("Name %1 already used").arg(connectionName));
        return;
    }

    QString driver = ui->driver->currentText();

    QSqlDatabase db = QSqlDatabase::addDatabase(driver,connectionName);

    db.setHostName(ui->host->text());
    db.setUserName(ui->user->text());
    db.setPassword(ui->password->text());
    db.setDatabaseName(ui->database->text());

    QString port = ui->port->text();

    if (!port.isEmpty()) {
        bool ok;
        int port_ = port.toInt(&ok);
        if (ok) {
            db.setPort(port_);
        }
    }

    if (!db.open()) {
        QString error = db.lastError().text();
        QMessageBox::critical(this,"Error",error);
        return;
    }

    QDialog::accept();
}


