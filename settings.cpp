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
    // m(.*) = (.*)$
    // obj["\1"] = m\1;
    obj["SavePasswords"] = mSavePasswords;
    obj["DateTimeFormat"] = mDateTimeFormat;
    obj["DateFormat"] = mDateFormat;
    obj["TimeFormat"] = mTimeFormat;
    obj["DateTimeLocale"] = mDateTimeLocale;
    obj["RealLocale"] = mRealLocale;
    saveJson(settingsPath(),obj);
}

void Settings::load()
{
    mSavePasswords = false;
    mDateTimeFormat = "yyyy-MM-dd hh:mm:ss";
    mDateFormat = "yyyy-MM-dd";
    mTimeFormat = "hh:mm:ss";
    mDateTimeLocale = DisplayFormat::Current;
    mRealLocale = DisplayFormat::Current;

    bool ok;
    QJsonDocument doc = loadJson(settingsPath(),&ok);
    if (doc.isNull()) {
        return;
    }
    QJsonObject obj = doc.object();

    mSavePasswords = obj.value("SavePasswords").toBool(false);
    mDateTimeFormat = obj.value("DateTimeFormat").toString();
    mDateFormat = obj.value("DateFormat").toString();
    mTimeFormat = obj.value("TimeFormat").toString();
    mDateTimeLocale = static_cast<DisplayFormat::Locale>(obj.value("DateTimeLocale").toInt(0));
    mRealLocale = static_cast<DisplayFormat::Locale>(obj.value("RealLocale").toInt(0));
}

/************************* GETTERS **************************/
bool Settings::savePasswords() const {
    return mSavePasswords;
}
QString Settings::dateTimeFormat() const {
    return mDateTimeFormat;
}
QString Settings::dateFormat() const {
    return mDateFormat;
}
QString Settings::timeFormat() const {
    return mTimeFormat;
}
DisplayFormat::Locale Settings::dateTimeLocale() const {
    return mDateTimeLocale;
}
DisplayFormat::Locale Settings::realLocale() const {
    return mRealLocale;
}
/************************* SETTERS **************************/
void Settings::setSavePasswords(bool value) {
    mSavePasswords = value;
}
void Settings::setDateTimeFormat(const QString& value) {
    mDateTimeFormat = value;
}
void Settings::setDateFormat(const QString& value) {
    mDateFormat = value;
}
void Settings::setTimeFormat(const QString& value) {
    mTimeFormat = value;
}
void Settings::setDateTimeLocale(DisplayFormat::Locale value) {
    mDateTimeLocale = value;
}
void Settings::setRealLocale(DisplayFormat::Locale value) {
    mRealLocale = value;
}
