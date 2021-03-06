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

#if QT_VERSION >= 0x050000
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#else
    QString appData = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

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
