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

QString AddDatabaseDialog::driver() {
    return ui->driver->currentText();
}

QString AddDatabaseDialog::host()
{
    return ui->host->text();
}

QString AddDatabaseDialog::user()
{
    return ui->user->text();
}

QString AddDatabaseDialog::password()
{
    return ui->password->text();
}

QString AddDatabaseDialog::database()
{
    return ui->database->text();
}

int AddDatabaseDialog::port()
{
    QString port = ui->port->text();
    if (!port.isEmpty()) {
        bool ok;
        int port_ = port.toInt(&ok);
        if (ok) {
            return port_;
        }
    }
    return -1;
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

    db.setHostName(host());
    db.setUserName(user());
    db.setPassword(password());
    db.setDatabaseName(database());
    if (port() > -1) {
        db.setPort(port());
    }

    if (!db.open()) {
        QString error = db.lastError().text();
        QMessageBox::critical(this,"Error",error);
        return;
    }

    QDialog::accept();
}

#include <QTableView>
#include <QSqlTableModel>
#include "databasehistorydialog.h"
#include <QDebug>

void AddDatabaseDialog::on_history_clicked()
{
    DatabaseHistoryDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {

        ui->connectionName->setText(dialog.connectionName());

        ui->driver->setCurrentIndex(ui->driver->findText(dialog.driver()));
        ui->host->setText(dialog.host());
        ui->user->setText(dialog.user());
        ui->password->setText(dialog.password());
        ui->database->setText(dialog.database());
        ui->port->setText(dialog.port() > -1 ? QString::number(dialog.port()) : QString());

    }

}
