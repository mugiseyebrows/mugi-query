#include "tools.h"

#include "settings.h"
#include <QFileDialog>
#include "toolmysqldialog.h"
#include <QProgressDialog>
#include <QApplication>
#include <QProcess>
#include <QMessageBox>
#include <QDebug>
#include "toolmysqldumpdialog.h"
#include <QDesktopServices>

#define MYSQL_TIMEOUT 600000
#define MYSQLDUMP_TIMEOUT 600000

static QStringList mysql_args(QSqlDatabase db) {
    QString databaseName = db.databaseName();
    QString userName = db.userName();
    QString password = db.password();
    QString hostName = db.hostName();

    QStringList args;

    if (!hostName.isEmpty()) {
        args.append("-h");
        args.append(hostName);
    }
    if (!userName.isEmpty())  {
        args.append("-u");
        args.append(userName);
    }
    if (!password.isEmpty()) {
        args.append("-p" + password);
    }
    if (!databaseName.isEmpty()) {
        args.append(databaseName);
    }
    return args;
}

static bool find_mysql(QWidget* widget, bool mysql) {
    Settings* settings = Settings::instance();
    QString path = mysql ? settings->mysqlPath() : settings->mysqldumpPath();
    if (path.isEmpty() || !QFile::exists(path)) {
        path = QFileDialog::getOpenFileName(widget, QString("Select %1.exe").arg(mysql ? "mysql" : "mysqldump"), QString(), "Executables (*.exe)");
        if (path.isEmpty()) {
            return false;
        }
        QString base = QFileInfo(path).baseName();
        if (base == "mysql") {
            settings->setMysqlPath(path);
            QString path2 = QFileInfo(path).dir().filePath("mysqldump.exe");
            if (QFile::exists(path2)) {
                settings->setMysqldumpPath(path2);
            }
        } else if (base == "mysqldump") {
            settings->setMysqldumpPath(path);
            QString path2 = QFileInfo(path).dir().filePath("mysql.exe");
            if (QFile::exists(path2)) {
                settings->setMysqlPath(path2);
            }
        }
    }
    return true;
}

void Tools::mysql(QSqlDatabase db, QWidget *widget)
{

    ToolMysqlDialog dialog(ToolMysqlDialog::OneFile, widget);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (!find_mysql(widget, true)) {
        return;
    }

    QString mysql = Settings::instance()->mysqlPath();

    QStringList inputs = dialog.inputs();

    if (inputs.isEmpty()) {
        return;
    }

    QProgressDialog progress(widget);
    progress.show();
    progress.setMaximum(inputs.size());
    qApp->processEvents();

    QStringList args = mysql_args(db);

    int complete = 0;
    for(const QString& input: qAsConst(inputs)) {
        QProcess process;
        process.setProgram(mysql);
        process.setArguments(args);

        process.setStandardInputFile(input);

        process.start();

        complete += 1;

        if (!process.waitForStarted()) {
            QMessageBox::critical(widget, "", QString("WaitForStarted error %1").arg(process.errorString()));
            continue;
        }

        if (!process.waitForFinished(MYSQL_TIMEOUT)) {
            QMessageBox::critical(widget, "Error", QString("WaitForFinished error %1").arg(process.errorString()));
        }

        QByteArray stderrData = process.readAllStandardError();

        if (!stderrData.isEmpty()) {
            QMessageBox::critical(widget, "Error", QString::fromUtf8(stderrData));
        }
        progress.setValue(complete);
        qApp->processEvents();
    }

    progress.close();
}

void Tools::mysqldump(QSqlDatabase db, QWidget *widget)
{
    ToolMysqldumpDialog dialog(db);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (!find_mysql(widget, false)) {
        return;
    }

    QString mysqldump = Settings::instance()->mysqldumpPath();

    QStringList tables = dialog.tables();
    bool schema = dialog.schema();
    bool data = dialog.data();

    QString output = dialog.output();
    if (!data && !schema) {
        return;
    }
    if (tables.isEmpty()) {
        return;
    }

    QStringList args = mysql_args(db);

    bool multipleFiles = dialog.multipleFiles();

    QDir dir;
    dir.mkpath(output);

    if (multipleFiles) {

        QString schema_base = QDir(output).filePath("schema");
        QString data_base = QDir(output).filePath("data");

        if (schema) {
            dir.mkpath(schema_base);
        }
        if (data) {
            dir.mkpath(data_base);
        }

        QProgressDialog progress;
        progress.setMaximum(tables.size());
        progress.show();
        qApp->processEvents();

        for(int i=0;i<tables.size();i++) {
            if (schema) {
                QString path = QDir(schema_base).filePath(QString("%1.sql").arg(tables[i]));
                QStringList args_ = args;
                args_.append("--no-data");
                args_.append(tables[i]);
                args_.append("--result-file");
                args_.append(QDir::toNativeSeparators(path));

                QProcess process;
                process.setProgram(mysqldump);
                process.setArguments(args_);
                process.start(QIODevice::ReadOnly);
                if (!process.waitForStarted()) {
                    QMessageBox::critical(widget, "", QString("WaitForStarted error %1").arg(process.errorString()));
                    return;
                }
                if (!process.waitForFinished(MYSQLDUMP_TIMEOUT)) {
                    QMessageBox::critical(widget, "", QString("WaitForFinished error %1").arg(process.errorString()));
                }
                QByteArray stderrData = process.readAllStandardError();
                if (!stderrData.isEmpty()) {
                    QMessageBox::critical(widget, "Error", QString::fromUtf8(stderrData));
                }
            }
            if (data) {
                QString path = QDir(data_base).filePath(QString("%1.sql").arg(tables[i]));
                QStringList args_ = args;
                args_.append("--no-create-info");
                args_.append(tables[i]);
                args_.append("--result-file");
                args_.append(QDir::toNativeSeparators(path));

                QProcess process;
                process.setProgram(mysqldump);
                process.setArguments(args_);
                process.start(QIODevice::ReadOnly);
                if (!process.waitForStarted()) {
                    QMessageBox::critical(widget, "Error", QString("WaitForStarted error %1").arg(process.errorString()));
                    return;
                }
                if (!process.waitForFinished(MYSQLDUMP_TIMEOUT)) {
                    QMessageBox::critical(widget, "Error", QString("WaitForFinished error %1").arg(process.errorString()));
                }
                QByteArray stderrData = process.readAllStandardError();
                if (!stderrData.isEmpty()) {
                    QMessageBox::critical(widget, "Error", QString::fromUtf8(stderrData));
                }
            }
            progress.setValue(i);
            qApp->processEvents();
        }

        QUrl url = QUrl::fromLocalFile(output);
        QDesktopServices::openUrl(url);

    } else {

        QProgressDialog progress;
        progress.show();
        qApp->processEvents();

        QString name = "dump.sql";
        if (schema && data) {
            name = "schema-and-data.sql";
        } else if (!schema) {
            name = "data.sql";
        } else if (!data) {
            name = "schema.sql";
        }

        QString path = QDir(output).filePath(name);
        QStringList args_ = args;
        if (!schema) {
            args_.append("--no-create-info");
        }
        if (!data) {
            args_.append("--no-data");
        }
        args_.append("--result-file");
        args_.append(QDir::toNativeSeparators(path));
        args_ = args_ + tables;

        QProcess process;
        process.setProgram(mysqldump);
        process.setArguments(args_);
        process.start(QIODevice::ReadOnly);
        if (!process.waitForStarted()) {
            QMessageBox::critical(widget, "Error", QString("WaitForStarted error %1").arg(process.errorString()));
            return;
        }

        if (!process.waitForFinished(MYSQLDUMP_TIMEOUT)) {
            QMessageBox::critical(widget, "Error", QString("WaitForFinished error %1").arg(process.errorString()));
        }

        QUrl url = QUrl::fromLocalFile(output);
        QDesktopServices::openUrl(url);
    }


}
