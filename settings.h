#ifndef SETTINGS_H
#define SETTINGS_H

/*
http://mugiseyebrows.ru/web-utils/class.html
Settings
bool savePasswords; QString dateTimeFormat; QString dateFormat; QString timeFormat; bool useLocaleDateFormat;

*/

#include <QString>
#include "displayformat.h"

class Settings
{
public:

    static Settings* instance();

    QString dir() const;

    QString settingsPath() const;

    void save();

    void load();

    /************************* GETTERS **************************/
    bool savePasswords() const;
    QString dateTimeFormat() const;
    QString dateFormat() const;
    QString timeFormat() const;
    DisplayFormat::Locale dateTimeLocale() const;
    DisplayFormat::Locale realLocale() const;
    /************************* SETTERS **************************/
    void setSavePasswords(bool value);
    void setDateTimeFormat(const QString& value);
    void setDateFormat(const QString& value);
    void setTimeFormat(const QString& value);
    void setDateTimeLocale(DisplayFormat::Locale value);
    void setRealLocale(DisplayFormat::Locale value);

private:

    Settings();

    static Settings* mInstance;

    QString mDateTimeFormat;
    QString mDateFormat;
    QString mTimeFormat;
    DisplayFormat::Locale mDateTimeLocale;
    DisplayFormat::Locale mRealLocale;

    QString mDir;
    bool mSavePasswords;

};

#endif // SETTINGS_H
