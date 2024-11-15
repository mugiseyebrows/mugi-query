#include "databaseconnectdialog.h"
#include "ui_databaseconnectdialog.h"

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QTableView>
#include <QSqlTableModel>
#include "databasehistorydialog.h"
#include <QDebug>
#include <QTimer>
#include "settings.h"
#include "automation.h"
#include "drivernames.h"
#include <QThread>
#include <QFileDialog>
#include "accessuri.h"

namespace {

QStringList availableSqlDrivers() {
    QStringList drivers = QSqlDatabase::drivers();
    drivers.removeAll("QMYSQL3");
    drivers.removeAll("QOCI8");
    drivers.removeAll("QODBC3");
    drivers.removeAll("QPSQL7");
    drivers.removeAll("QTDS7");
    return drivers;
}

}

DatabaseConnectDialog::DatabaseConnectDialog(bool showHistory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseConnectDialog)
{
    ui->setupUi(this);

    ui->driver->addItems(availableSqlDrivers());

    this->adjustSize();

    if (showHistory) {
        QTimer::singleShot(0,this,SLOT(on_history_clicked()));
    }

    ui->savePassword->setChecked(Settings::instance()->savePasswords());
}

DatabaseConnectDialog::~DatabaseConnectDialog()
{
    delete ui;
}

QString DatabaseConnectDialog::connectionName() {
    return ui->connectionName->text();
}

QString DatabaseConnectDialog::driver() {
    return ui->driver->currentText();
}

QString DatabaseConnectDialog::host()
{
    return ui->host->text();
}

QString DatabaseConnectDialog::user()
{
    return ui->user->text();
}

QString DatabaseConnectDialog::password()
{
    return ui->password->text();
}

QString DatabaseConnectDialog::database()
{
    return ui->database->text();
}

int DatabaseConnectDialog::port()
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

void DatabaseConnectDialog::accept()
{
    //qDebug() << "AddDatabaseDialog::accept()" << QThread::currentThreadId();

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
        QSqlDatabase::removeDatabase(connectionName);
        return;
    }

    Settings::instance()->setSavePasswords(ui->savePassword->isChecked());

    QDialog::accept();
}

void DatabaseConnectDialog::on_history_clicked()
{
    DatabaseHistoryDialog dialog;

    Automation::instance()->beforeDialog(&dialog);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    //qDebug() << "AddDatabaseDialog::on_history_clicked()" << QThread::currentThreadId();

    ui->connectionName->setText(dialog.connectionName());
    ui->driver->setCurrentIndex(ui->driver->findText(dialog.driver()));
    ui->host->setText(dialog.host());
    ui->user->setText(dialog.user());
    ui->password->setText(dialog.password());
    ui->database->setText(dialog.database_());
    ui->port->setText(dialog.port() > -1 ? QString::number(dialog.port()) : QString());

    Automation::instance()->afterDialog(&dialog);
}

void DatabaseConnectDialog::on_savePassword_clicked(bool checked)
{
    if (checked) {
        QString msg("Storing passwords is not secure, are you sure?");
        int res = QMessageBox::question(this,"",msg,QMessageBox::Yes,QMessageBox::No);
        if (res != QMessageBox::Yes) {
            ui->savePassword->setChecked(false);
        }
    }
    Settings::instance()->setSavePasswords(checked);
}

void DatabaseConnectDialog::on_pick_clicked()
{
    if (ui->driver->currentText() == DRIVER_SQLITE) {
        QString path = QFileDialog::getOpenFileName(this);
        if (path.isEmpty()) {
            return;
        }
        ui->database->setText(QDir::toNativeSeparators(path));
        ui->connectionName->setText(QFileInfo(path).baseName());
    } else if (ui->driver->currentText() == DRIVER_ODBC) {

        QString xls = "*.xls *.xlsx *.xlsm *.xlsb";
        QString csv = "*.txt *.csv";
        QString mdb = "*.mdb *.accdb";
        QString filter1 = QString("Data files (%1 %2 %3)").arg(mdb).arg(csv).arg(xls);
        QString filter2 = QString("Access Databases (%1)").arg(mdb);
        QString filter3 = QString("Csv Tables (%1)").arg(csv);
        QString filter4 = QString("Excel files (%1)").arg(xls);

        QStringList filter = {filter1, filter2, filter3, filter4};

        QString path = QFileDialog::getOpenFileName(this, QString(), QString(), filter.join(";;"));
        if (path.isEmpty()) {
            return;
        }
        QFileInfo info(path);
        QString suffix = info.suffix().toLower();

        QString uri;
        QString connectionName;

        if (suffix == "mdb" || suffix == "accdb") {
            uri = accessUri(path);
            connectionName = info.baseName();
        } else if (suffix.startsWith("xls")) {
            uri = excelUri(path);
            connectionName = info.baseName();
        } else if (suffix == "csv" || suffix == "txt") {
            uri = csvUri(path);
            connectionName = info.dir().dirName();
        } else {
            qDebug() << "on_pick_clicked error" << __FILE__ << __LINE__;
            return;
        }

        ui->database->setText(uri);

        if (ui->connectionName->text() == "untitled" || ui->connectionName->text() == "") {
            ui->connectionName->setText(connectionName);
        }

    }
}

#if 0
void DatabaseConnectDialog::on_driver_currentIndexChanged(const QString &name)
{
    ui->pick->setVisible(name == DRIVER_ODBC || name == DRIVER_SQLITE);
}
#endif

void DatabaseConnectDialog::on_driver_currentTextChanged(const QString &name)
{
    ui->pick->setVisible(name == DRIVER_ODBC || name == DRIVER_SQLITE);
}

