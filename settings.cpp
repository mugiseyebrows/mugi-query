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
    obj["SavePasswords"] = mSavePasswords;
    obj["DateTimeFormat"] = mDateTimeFormat;
    saveJson(settingsPath(),obj);
}

void Settings::load()
{
    mSavePasswords = false;
    mDateTimeFormat = DateTimeFormatWithSeconds;

    bool ok;
    QJsonDocument doc = loadJson(settingsPath(),&ok);
    if (doc.isNull()) {
        return;
    }
    QJsonObject obj = doc.object();

    mSavePasswords = obj.value("SavePasswords").toBool(false);
    mDateTimeFormat = static_cast<DateTimeFormat>(obj.value("DateTimeFormat").toInt());
}

bool Settings::savePasswords() const
{
    return mSavePasswords;
}

void Settings::setSavePasswords(bool value)
{
    mSavePasswords = value;
}
