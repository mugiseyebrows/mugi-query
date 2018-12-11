#include "settings.h"

Settings* Settings::mInstance = 0;

#include <QDir>
#include <QApplication>
#include <QStandardPaths>
#include <QMessageBox>
#include "jsonhelper.h"
#include <QJsonDocument>
#include <QJsonObject>

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

Settings::DateTimeFormat Settings::dateTimeFormat() const
{
    return mDateTimeFormat;
}

void Settings::setDateTimeFormat(Settings::DateTimeFormat format)
{
    mDateTimeFormat = format;
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
    obj["SavePasswords"] = QJsonValue(mSavePasswords);
    saveJson(settingsPath(),obj);
}

void Settings::load()
{
    mSavePasswords = false;

    bool ok;
    QJsonDocument doc = loadJson(settingsPath(),&ok);
    if (doc.isNull()) {
        return;
    }
    QJsonObject obj = doc.object();

    mSavePasswords = obj.value("SavePasswords").toBool(false);

}

bool Settings::savePasswords() const
{
    return mSavePasswords;
}

void Settings::setSavePasswords(bool value)
{
    mSavePasswords = value;
}
