#include "settings.h"

Settings* Settings::mInstance = 0;

#include <QDir>
#include <QApplication>
#include <QStandardPaths>
#include <QMessageBox>
#include "jsonhelper.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

Settings::Settings()
{
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString name = qApp->applicationName();
    QDir d(appData);
    if (!d.exists()) {
        d.cdUp();
        if (!d.mkdir(name)) {
            QString error = QString("Can not create directory %1").arg(appData);
            QMessageBox::critical(qApp->activeWindow(),"Error",error);
        }
        d.cd(name);
    }
    mDir = appData;
    load();
    findTools();
}

Settings *Settings::instance()
{
    if (!mInstance) {
        mInstance = new Settings();
    }
    return mInstance;
}

QString Settings::dir() const
{
    return mDir;
}

QString Settings::settingsPath() const
{
    return QDir(mDir).filePath("settings.json");
}


void Settings::save()
{
    QJsonObject obj;
    // (.*) m(.*);$
    // obj["\2"] = m\2;
    obj["SavePasswords"] = mSavePasswords;
    obj["DateTimeOverrideForCsv"] = mDateTimeOverrideForCsv;
    obj["DateTimeOverrideForCopy"] = mDateTimeOverrideForCopy;
    obj["RealOverrideForCopy"] = mRealOverrideForCopy;
    obj["RealOverrideForCsv"] = mRealOverrideForCsv;
    obj["RealUseLocale"] = mRealUseLocale;
    obj["DateFormat"] = mDateFormat;
    obj["TimeFormat"] = mTimeFormat;
    obj["DateTimeUseLocale"] = mDateTimeUseLocale;
    obj["MysqlPath"] = mMysqlPath;
    obj["MysqldumpPath"] = mMysqldumpPath;
    obj["HomePath"] = mHomePath;
    saveJson(settingsPath(),obj);
}

void loadValue(const QJsonObject& obj, const QString& name, bool* v) {
    if (obj.contains(name)) {
        *v = obj[name].toBool();
    }
}

void loadValue(const QJsonObject& obj, const QString& name, QString* v) {
    if (obj.contains(name)) {
        *v = obj[name].toString();
    }
}

void Settings::load()
{
    mSavePasswords = false;
    mDateFormat = "yyyy-MM-dd";
    mTimeFormat = "hh:mm:ss";
    mDateTimeUseLocale = true;
    mDateTimeOverrideForCsv = false;
    mDateTimeOverrideForCopy = false;
    mRealUseLocale = false;
    mRealOverrideForCopy = false;
    mRealOverrideForCsv = false;

    mHomePath = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).filePath("mugi-query");

    bool ok;
    QJsonDocument doc = loadJson(settingsPath(),&ok);
    if (doc.isNull()) {
        return;
    }
    QJsonObject obj = doc.object();
    // (.*) m(.*);$
    // loadValue(obj,"\2",&m\2);
    loadValue(obj,"SavePasswords",&mSavePasswords);
    loadValue(obj,"DateTimeOverrideForCsv",&mDateTimeOverrideForCsv);
    loadValue(obj,"DateTimeOverrideForCopy",&mDateTimeOverrideForCopy);
    loadValue(obj,"RealOverrideForCopy",&mRealOverrideForCopy);
    loadValue(obj,"RealOverrideForCsv",&mRealOverrideForCsv);
    loadValue(obj,"RealUseLocale",&mRealUseLocale);
    loadValue(obj,"DateFormat",&mDateFormat);
    loadValue(obj,"TimeFormat",&mTimeFormat);
    loadValue(obj,"DateTimeUseLocale",&mDateTimeUseLocale);
    loadValue(obj,"MysqlPath",&mMysqlPath);
    loadValue(obj,"MysqldumpPath",&mMysqldumpPath);
    loadValue(obj,"HomePath", &mHomePath);
    mHomePath = QDir::toNativeSeparators(mHomePath);
}

/************************* GETTERS **************************/
bool Settings::savePasswords() const {
    return mSavePasswords;
}
bool Settings::dateTimeOverrideForCsv() const {
    return mDateTimeOverrideForCsv;
}
bool Settings::dateTimeOverrideForCopy() const {
    return mDateTimeOverrideForCopy;
}
bool Settings::realOverrideForCopy() const {
    return mRealOverrideForCopy;
}
bool Settings::realOverrideForCsv() const {
    return mRealOverrideForCsv;
}
bool Settings::realUseLocale() const {
    return mRealUseLocale;
}
QString Settings::dateFormat() const {
    return mDateFormat;
}
QString Settings::timeFormat() const {
    return mTimeFormat;
}
bool Settings::dateTimeUseLocale() const {
    return mDateTimeUseLocale;
}
/************************* SETTERS **************************/
void Settings::setSavePasswords(bool value) {
    mSavePasswords = value;
}
void Settings::setDateTimeOverrideForCsv(bool value) {
    mDateTimeOverrideForCsv = value;
}
void Settings::setDateTimeOverrideForCopy(bool value) {
    mDateTimeOverrideForCopy = value;
}
void Settings::setRealOverrideForCopy(bool value) {
    mRealOverrideForCopy = value;
}
void Settings::setRealOverrideForCsv(bool value) {
    mRealOverrideForCsv = value;
}
void Settings::setRealUseLocale(bool value) {
    mRealUseLocale = value;
}
void Settings::setDateFormat(const QString& value) {
    mDateFormat = value;
}
void Settings::setTimeFormat(const QString& value) {
    mTimeFormat = value;
}
void Settings::setDateTimeUseLocale(bool value) {
    mDateTimeUseLocale = value;
}

QString Settings::mysqlPath() const
{
    return mMysqlPath;
}

void Settings::setMysqlPath(const QString &path)
{
    mMysqlPath = path;
}

QString Settings::mysqldumpPath() const
{
    return mMysqldumpPath;
}

void Settings::setMysqldumpPath(const QString &path)
{
    mMysqldumpPath = path;
}

static QString pathJoin(const QStringList path) {
    QDir dir(path[0]);
    for(int i=1;i<path.size();i++) {
        dir = QDir(dir.filePath(path[i]));
    }
    return dir.path();
}

static QString existing(const QStringList& bases, const QString& path) {
    for(const QString& base: bases) {
        if (base.isEmpty()) {
            continue;
        }
        QString path_ = pathJoin({base, path});
        if (QFileInfo(path_).exists()) {
            return QDir::toNativeSeparators(path_);
        }
    }
    return QString();
}

void Settings::findTools()
{
    if (mMysqlPath.isEmpty() || !QFile::exists(mMysqlPath)) {
        QString path = existing({"C:\\Program Files\\MariaDB 10.10\\bin"}, "mysql.exe");
        if (path.isEmpty()) {
            mMysqlPath = QString();
            return;
        }
        mMysqlPath = path;
    }
    if (mMysqldumpPath.isEmpty() || !QFile::exists(mMysqldumpPath)) {
        QString path = existing({"C:\\Program Files\\MariaDB 10.10\\bin"}, "mysqldump.exe");
        if (path.isEmpty()) {
            mMysqldumpPath = QString();
            return;
        }
        mMysqldumpPath = path;
    }
}

void Settings::setHomePath(const QString &path)
{
    mHomePath = path;
}

QString Settings::homePath() const
{
    return mHomePath;
}
