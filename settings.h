#ifndef SETTINGS_H
#define SETTINGS_H


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

private:

    Settings();

    static Settings* mInstance;

    DateTimeFormat mDateTimeFormat;

};

#endif // SETTINGS_H
