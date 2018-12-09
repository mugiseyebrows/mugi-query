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

private:

    Settings();

    static Settings* mInstance;

    DateTimeFormat mDateTimeFormat;
    QString mDir;

};

#endif // SETTINGS_H
