#ifndef SETTINGS_H
#define SETTINGS_H

/*
http://mugiseyebrows.ru/web-utils/class.html
Settings
bool savePasswords; bool dateTimeOverrideForCsv; bool dateTimeOverrideForCopy; bool realOverrideForCopy; bool realOverrideForCsv; bool realUseLocale; QString dateFormat; QString timeFormat; bool dateTimeUseLocale

*/

#include <QString>


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
    bool dateTimeOverrideForCsv() const;
    bool dateTimeOverrideForCopy() const;
    bool realOverrideForCopy() const;
    bool realOverrideForCsv() const;
    bool realUseLocale() const;
    QString dateFormat() const;
    QString timeFormat() const;
    bool dateTimeUseLocale() const;
    /************************* SETTERS **************************/
    void setSavePasswords(bool value);
    void setDateTimeOverrideForCsv(bool value);
    void setDateTimeOverrideForCopy(bool value);
    void setRealOverrideForCopy(bool value);
    void setRealOverrideForCsv(bool value);
    void setRealUseLocale(bool value);
    void setDateFormat(const QString& value);
    void setTimeFormat(const QString& value);
    void setDateTimeUseLocale(bool value);

    QString mysqlPath() const;
    void setMysqlPath(const QString& path);
    QString mysqldumpPath() const;
    void setMysqldumpPath(const QString& path);

    void findTools();

    void setHomePath(const QString& path);
    QString homePath() const;

private:

    Settings();

    static Settings* mInstance;

    bool mSavePasswords;
    bool mDateTimeOverrideForCsv;
    bool mDateTimeOverrideForCopy;
    bool mRealOverrideForCopy;
    bool mRealOverrideForCsv;
    bool mRealUseLocale;
    QString mDateFormat;
    QString mTimeFormat;
    bool mDateTimeUseLocale;
    QString mMysqlPath;
    QString mMysqldumpPath;
    QString mHomePath;

    QString mDir;

};

#endif // SETTINGS_H
