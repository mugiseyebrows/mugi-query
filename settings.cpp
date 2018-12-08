#include "settings.h"

Settings* Settings::mInstance = 0;

Settings::Settings()
{

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
