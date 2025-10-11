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
#include "schema2data.h"
#include <QFile>
#include <QTextStream>
#include "schema2tablesmodel.h"

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
#define PUSHCSV_TIMEOUT 600000

static QStringList mysql_args(QSqlDatabase db, bool ssl, bool force = false) {
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
    if (force) {
        args.append("--force");
    }
    return args;
}

static QStringList mysqldump_args(QSqlDatabase db, const MysqldumpSettings& settings,
                                  const QStringList& tables, const QString& resultFile) {
    QStringList args = mysql_args(db, settings.ssl);
    if (!settings.schema) {
        args.append("--no-create-info");
    }
    if (!settings.data) {
        args.append("--no-data");
    }
    if (settings.routines) {
        args.append("--routines");
    }
    if (settings.completeInsert) {
        args.append("--complete-insert");
    }
    if (settings.insertIgnore) {
        args.append("--insert-ignore");
    }
    if (settings.extendedInsert) {
        args.append("--extended-insert");
    } else {
        args.append("--skip-extended-insert");
    }
    if (settings.hexBlob) {
        args.append("--hex-blob");
    }
    if (settings.quoteNames) {
        args.append("--quote-names");
    } else {
        args.append("--skip-quote-names");
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

    QStringList args = mysql_args(db, dialog.ssl(), dialog.force());

    int complete = 0;
    for(const QString& path: std::as_const(files)) {
        execute(mysql, args, path, MYSQL_TIMEOUT, widget);
        progress.setValue(complete);
        qApp->processEvents();
    }

    progress.close();
}

static QStringList sortedInInsertOrder(Schema2Data *data, const QStringList& tables) {
    if (tables.size() < 2) {
        return tables;
    }
    SNames tableNames = data->tables()->tableNames();
    QStringList notFound;
    SNames tables_;
    for(const QString& table: tables) {
        int index = indexOf(tableNames, table);
        if (index > -1) {
            tables_.append(tableNames[index]);
        } else {
            qDebug() << "not found table" << table << __FILE__ << __LINE__;
            notFound.append(table);
        }
    }
    auto sorted = data->sortedInInsertOrder(tables_);
    return notFound + sorted.getNames();
}

static void backupFile(const QString& path) {
    QFileInfo info(path);
    if (!info.exists()) {
        return;
    }
    QString baseName = info.completeBaseName();
    int i = 1;
    QString newName = path;
    while (QFile(newName).exists()) {
        newName = info.absoluteDir().filePath(QString("%1-%2.bak").arg(baseName).arg(i, 3, 10, QChar('0')));
        i++;
    }
    info.absoluteDir().rename(path, newName);
}

void Tools::mysqldump(Schema2Data *data, QSqlDatabase db, QWidget *widget)
{
    if (!find_mysql(widget, false)) {
        return;
    }

    Settings* settings = Settings::instance();

    MysqldumpSettings mysqldumpSettings1 = settings->mysqldumpSettings();

    ToolMysqldumpDialog dialog(db, mysqldumpSettings1, widget);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString mysqldump = settings->mysqldumpPath();

    MysqldumpSettings mysqldumpSettings = dialog.settings();

    settings->setMysqldumpSettings(mysqldumpSettings);

    QString connectionName = db.connectionName();
    QStringList tables = mysqldumpSettings.tables;
    QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd_hhmmss");

    QString resultDir;
    switch(mysqldumpSettings.path) {
    case MysqldumpSettings::Table:
        resultDir = mysqldumpSettings.output;
        break;
    case MysqldumpSettings::DatabaseTable:
        resultDir = pathJoin({mysqldumpSettings.output, connectionName});
        break;
    case MysqldumpSettings::DatabaseDatetimeTable:
        resultDir = pathJoin({mysqldumpSettings.output, connectionName, dateTime});
        break;
    default:
        qDebug() << "not implemented" << mysqldumpSettings.path << __FILE__ << __LINE__;
        break;
    }

    QDir dir(resultDir);
    if (!dir.exists()) {
        dir.mkpath(resultDir);
    }

    if (mysqldumpSettings.format == MysqldumpSettings::OneFile) {

        QString name = mysqldumpSettings.oneFileName;

        tables = sortedInInsertOrder(data, tables);

        QString resultFile = pathJoin({resultDir, name});

        backupFile(resultFile);

        QProgressDialog progress;
        progress.setMaximum(0);
        progress.show();

        QStringList args = mysqldump_args(db, mysqldumpSettings, tables, resultFile);
        execute(mysqldump, args, QString(), MYSQLDUMP_TIMEOUT, widget);

    } else if (mysqldumpSettings.format == MysqldumpSettings::MultipleFiles) {
        QStringList resultFiles;
        for(const QString& table: tables) {
            QString path = pathJoin({resultDir, table + ".sql"});
            resultFiles.append(path);
            backupFile(path);
        }

        QProgressDialog progress;
        progress.setMaximum(tables.size());
        progress.show();
        qApp->processEvents();
        for(int i=0;i<tables.size();i++) {
            QStringList args = mysqldump_args(db, mysqldumpSettings, tables.mid(i, 1), resultFiles[i]);
            execute(mysqldump, args, QString(), MYSQLDUMP_TIMEOUT, widget);
            progress.setValue(i);
            progress.setLabelText(tables[i]);
            qApp->processEvents();
        }
        progress.close();
    } else {
        qDebug() << "not implemented" << mysqldumpSettings.format << __FILE__ << __LINE__;
    }

}

void Tools::pushCsv(QSqlDatabase db, const QString &path, const QString& name, IfExists ifExists, QWidget *widget)
{
    QString python = Settings::instance()->pythonPath();
    if (python.isEmpty()) {
        QMessageBox::critical(widget, "Error", "Python not found");
        return;
    }
    QDir dir(qApp->applicationDirPath());
    if (dir.dirName().toLower().endsWith("debug")) {
        dir.cdUp();
        dir.cd("src");
    }
    QString script = dir.filePath("push-csv.py");
    if (!QFile(script).exists()) {
        QMessageBox::critical(widget, "Error", "push-csv.py not found");
        return;
    }
    QStringList args = {QDir::toNativeSeparators(script),
                        "--type", "mysql",
                        "--user", db.userName(),
                        "--password", db.password(),
                        "--host", db.hostName(),
                        "--database", db.databaseName(),
                        "--name", name};
    switch (ifExists) {
    case IfExists::Fail: args.append("--fail"); break;
    case IfExists::Replace: args.append("--replace"); break;
    case IfExists::Append: args.append("--append"); break;
    }
    args.append(path);
    execute(python, args, {}, PUSHCSV_TIMEOUT, widget);
}
