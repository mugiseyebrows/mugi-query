#include "formats.h"

#include "settings.h"

Formats::Formats(DataFormat::ActionType action)
{
    Settings* s = Settings::instance();

    dateTimeUseLocale = true;
    dateFormat = s->dateFormat();
    timeFormat = s->timeFormat();
    realUseLocale = false;

    if (!s->dateTimeUseLocale()
            && ((action == DataFormat::ActionCopy && s->dateTimeOverrideForCopy())
                || (action == DataFormat::ActionSave && s->dateTimeOverrideForCsv()))) {
        dateFormat = s->dateFormat();
        timeFormat = s->timeFormat();
        dateTimeUseLocale = false;
    }

    if (s->realUseLocale() && ((action == DataFormat::ActionCopy && s->realOverrideForCopy())
                               || (action == DataFormat::ActionSave && s->realOverrideForCsv()))) {
        realUseLocale = true;
    }
}
