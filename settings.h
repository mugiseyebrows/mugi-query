#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings
{
public:
    enum DateTimeFormat {
        DateTimeFormatWithSeconds,
        DateTimeFormatWitoutSeconds
    };

    static Settings* instance();

    DateTimeFormat dateTimeFormat() const;

    void setDateTimeFormat(DateTimeFormat format);

    QString dir() const;

    QString settingsPath() const;

    void save();

    void load();

    bool savePasswords() const;
    void setSavePasswords(bool value);

private:

    Settings();

    static Settings* mInstance;

    DateTimeFormat mDateTimeFormat;
    QString mDir;
    bool mSavePasswords;

};

#endif // SETTINGS_H
