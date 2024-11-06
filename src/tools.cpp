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

#include <QFile>
#include <QTextStream>

void dump(const QString& path, const QStringList& lines) {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        qDebug() << "!open" << path;
        return;
    }
    QTextStream stream(&f);
    stream.setEncoding(QStringConverter::Utf8);
    foreach(const QString& line, lines) {
        stream << line << "\n";
    }
    qDebug() << "text writen to" << path;
}

static QString pathJoin(const QStringList& args) {
    QStringList res;
    for(const QString& arg: args) {
        if (res.isEmpty()) {
            res.append(arg);
            continue;
        }
        if (res[res.size()-1].endsWith("/")
                || res[res.size()-1].endsWith("\\")
                || arg.startsWith("/")
                || arg.startsWith("\\")) {
            res.append(arg);
        } else {
            res.append(QDir::separator());
            res.append(arg);
        }
    }
    return res.join("");
}

#define MYSQL_TIMEOUT 600000
#define MYSQLDUMP_TIMEOUT 600000

static QStringList mysql_args(QSqlDatabase db, bool ssl) {
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
    if (!ssl) {
        args.append("--ssl=FALSE");
    }
    return args;
}

static QStringList mysqldump_args(QSqlDatabase db, bool ssl, bool schema, bool data,
                                  const QStringList& tables, const QString& resultFile) {
    QStringList args = mysql_args(db, ssl);
    if (!schema) {
        args.append("--no-create-info");
    }
    if (!data) {
        args.append("--no-data");
    }
    args.append("--result-file");
    args.append(resultFile);
    for(const QString& table: tables) {
        args.append(table);
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

static void execute(const QString& program, const QStringList& args, const QString& inputFile, int timeout, QWidget *widget) {

    QProcess process;
    process.setProgram(program);
    process.setArguments(args);

    if (!inputFile.isEmpty()) {
        process.setStandardInputFile(inputFile);
    }

    process.start(QIODevice::ReadOnly);
    if (!process.waitForStarted()) {
        QMessageBox::critical(widget, QString(), QString("WaitForStarted error %1").arg(process.errorString()));
        return;
    }
    if (!process.waitForFinished(timeout)) {
        QMessageBox::critical(widget, QString(), QString("WaitForFinished error %1").arg(process.errorString()));
    }
    QByteArray stderrData = process.readAllStandardError();
    if (!stderrData.isEmpty()) {
        QMessageBox::critical(widget, QString(), QString::fromUtf8(stderrData));
    }

}


void Tools::mysql(QSqlDatabase db, QWidget *widget)
{

    ToolMysqlDialog dialog(widget);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    if (!find_mysql(widget, true)) {
        return;
    }

    QString mysql = Settings::instance()->mysqlPath();

    QStringList files = dialog.files();

    if (files.isEmpty()) {
        return;
    }

    QProgressDialog progress(widget);
    progress.show();
    progress.setMaximum(files.size());
    qApp->processEvents();

    QStringList args = mysql_args(db, dialog.ssl());

    int complete = 0;
    for(const QString& path: std::as_const(files)) {
        execute(mysql, args, path, MYSQL_TIMEOUT, widget);
#if 0
        QProcess process;
        process.setProgram(mysql);
        process.setArguments(args);

        process.setStandardInputFile(path);

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
#endif
        progress.setValue(complete);
        qApp->processEvents();
    }

    progress.close();
}




void Tools::mysqldump(QSqlDatabase db, QWidget *widget)
{
    if (!find_mysql(widget, false)) {
        return;
    }

    ToolMysqldumpDialog dialog(db, widget);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString mysqldump = Settings::instance()->mysqldumpPath();

    MysqldumpSettings settings = dialog.settings();
    QString connectionName = db.connectionName();
    QStringList tables = settings.tables;
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hhmmss");

    QString resultDir;
    switch(settings.path) {
    case MysqldumpSettings::DatabaseName:
        resultDir = pathJoin({settings.output, connectionName});
        break;
    case MysqldumpSettings::DatabaseDatetimeName:
        resultDir = pathJoin({settings.output, connectionName, dateTime});
        break;
    default:
        qDebug() << "not implemented" << settings.path << __FILE__ << __LINE__;
        break;
    }

    QDir dir(resultDir);
    if (!dir.exists()) {
        dir.mkpath(resultDir);
    }

    if (settings.format == MysqldumpSettings::OneFile) {

        QString name;
        if (settings.data && settings.schema) {
            name = "dump";
        } else if (settings.data) {
            name = "data";
        } else if (settings.schema) {
            name = "schema";
        }

        QString resultFile = pathJoin({resultDir, name + ".sql"});

        QProgressDialog progress;
        progress.setMaximum(0);
        progress.show();

        QStringList args = mysqldump_args(db, settings.ssl, settings.schema, settings.data, tables, resultFile);
        execute(mysqldump, args, QString(), MYSQLDUMP_TIMEOUT, widget);

    } else if (settings.format == MysqldumpSettings::MultipleFiles) {

        QStringList resultFiles;

        for(const QString& table: tables) {
            QString path = pathJoin({resultDir, table + ".sql"});
            resultFiles.append(path);
        }

        QProgressDialog progress;
        progress.setMaximum(tables.size());
        progress.show();
        qApp->processEvents();
        for(int i=0;i<tables.size();i++) {
            QStringList args = mysqldump_args(db, settings.ssl, settings.schema, settings.data, tables.mid(i, 1), resultFiles[i]);
            execute(mysqldump, args, QString(), MYSQLDUMP_TIMEOUT, widget);
            progress.setValue(i);
            progress.setLabelText(tables[i]);
            qApp->processEvents();
        }
        progress.close();
    } else {
        qDebug() << "not implemented" << settings.format << __FILE__ << __LINE__;
    }


#if 0
    QStringList tables = dialog.tables();
    bool schema = dialog.schema();
    bool data = dialog.data();
    bool tab = dialog.tab();
    bool ssl = dialog.ssl();
    QString output = dialog.output();
    if (!data && !schema) {
        return;
    }
    if (tables.isEmpty()) {
        return;
    }

    QStringList args = mysql_args(db);
    if (!ssl) {
        args.append("--ssl=FALSE");
    }

    bool multipleFiles = dialog.multipleFiles();

    QDir dir;
    dir.mkpath(output);

    if (tab) {

        QStringList args_ = args;
        args_.append("--tab");
        args_.append(QDir::toNativeSeparators(output));

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
        QByteArray stderrData = process.readAllStandardError();
        if (!stderrData.isEmpty()) {
            QMessageBox::critical(widget, "Error", QString::fromUtf8(stderrData));
        }

        QUrl url = QUrl::fromLocalFile(output);
        QDesktopServices::openUrl(url);

    } else if (multipleFiles) {

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

        //dump("D:\\w\\mysqdump.txt", {args_.join(" ")}); return;

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
#endif

}
