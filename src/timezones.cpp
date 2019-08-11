#include "timezones.h"
#include <QMap>
#include "multinameenum.h"
#include <QRegularExpression>
#include <QDebug>

namespace {

QString head(const QString& s) {
    return "^" + s;
}

QString tail(const QString& s) {
    return s + "$";
}

QString whole(const QString& s) {
    return "^" + s + "$";
}

QString group(const QString& s, bool cap = true) {
    return (cap ? "(" : "(?:") + s + ")";
}

QString group(const QStringList& s, bool cap = true) {
    return (cap ? "(" : "(?:") + s.join("|") + ")";
}

}

/*static*/
QMap<QString,QByteArray> TimeZones::mZones1 = {};
/*static*/
QMap<QString,TimeZone> TimeZones::mZones2 = {};

TimeZones::TimeZones()
{

}

QStringList escaped(const QStringList& vs) {
    QStringList res;
    foreach(const QString& s, vs) {
        QString s_ = s;
        s_.replace("+","\\+");
        res << s_;
    }
    return res;
}

QString TimeZones::regExp()
{
    init();
    MultinameEnum enum_(escaped(mZones1.keys()),mZones2.keys());
    QString tzNumeric = "[+][0-9]{4}";
    QString tzRtz = "RTZ\\s[0-9]+\\s[(]зима[)]";
    return group({group(tzNumeric,false),group(tzRtz,false),enum_.regExp(false)});
}

QString TimeZones::parseTimeZone(const QString& s, QDateTime& dateTime, bool* hasTimeZone) {

    if (s.isEmpty()) {
        *hasTimeZone = false;
        return QString();
    }

    QRegularExpression rx(whole(regExp()));
    QRegularExpressionMatch m = rx.match(s);
    *hasTimeZone = m.hasMatch();
    if (m.hasMatch()) {
        QTimeZone timeZone = parseTimeZone(m.captured(1));
        if (timeZone.isValid()) {
            dateTime = QDateTime(dateTime.date(), dateTime.time(), timeZone);
        } else {
            qDebug() << "invalid timezone" << m.captured();
            *hasTimeZone = false;
        }
        return QString();
    }
    return s;
}

QTimeZone TimeZones::parseTimeZone(const QString& timeZoneCode) {

    QRegularExpression tzNumeric("[+]([0-9]{2})([0-9]{2})");
    QRegularExpression tzRtz("RTZ\\s([0-9]+)\\s[(]зима[)]");

    QRegularExpressionMatch m = tzNumeric.match(timeZoneCode);
    if (m.hasMatch()) {
        int hours = m.captured(1).toInt();
        int minutes = m.captured(2).toInt();
        return QTimeZone(hours * 3600 + minutes * 60);
    }
    m = tzRtz.match(timeZoneCode);

    if (m.hasMatch()) {
        int rtz = m.captured(1).toInt();
        return QTimeZone((rtz + 1) * 3600);
    }

    if (mZones1.contains(timeZoneCode)) {
        return QTimeZone(mZones1[timeZoneCode]);
    }

    if (mZones2.contains(timeZoneCode)) {
        TimeZone timeZoneData = mZones2[timeZoneCode];
        QTimeZone timeZone =  QTimeZone(timeZoneData.ianaId());
        if (!timeZone.isValid()) {
            timeZone = QTimeZone(timeZoneData.offset());
        }
        return timeZone;
    }

    return QTimeZone();
}

void TimeZones::init()
{
    QDateTime dateTime = QDateTime::currentDateTime();

    if (mZones1.isEmpty()) {
        QList<QByteArray> timeZoneIds = QTimeZone::availableTimeZoneIds();
        foreach (const QByteArray& timeZoneId, timeZoneIds) {
            QTimeZone timeZone(timeZoneId);
            QString abbreviation = timeZone.abbreviation(dateTime);
            if (!mZones1.contains(abbreviation)) {
                mZones1[abbreviation] = timeZoneId;
            }
        }
    }

    // TODO: store in json
    if (mZones2.isEmpty()) {
        mZones2 = {
            {"A", TimeZone("A", "Alpha Time Zone", "UTC+01:00", "UTC +1", 3600)},
            {"ACST", TimeZone("ACST", "Australian Central Standard Time", "UTC+09:30", "UTC +9:30", 34200)},
            {"ACWST", TimeZone("ACWST", "Australian Central Western Standard Time", "Australia/Eucla", "UTC +8:45", 31500)},
            {"AEDT", TimeZone("AEDT", "Australian Eastern Daylight Time", "UTC+11:00", "UTC +11", 39600)},
            {"AEST", TimeZone("AEST", "Australian Eastern Standard Time", "UTC+10:00", "UTC +10", 36000)},
            {"AFT", TimeZone("AFT", "Afghanistan Time", "Asia/Kabul", "UTC +4:30", 16200)},
            {"AKDT", TimeZone("AKDT", "Alaska Daylight Time", "UTC-08:00", "UTC -8", -28800)},
            {"AKST", TimeZone("AKST", "Alaska Standard Time", "UTC-09:00", "UTC -9", -32400)},
            {"ALMT", TimeZone("ALMT", "Alma-Ata Time", "Asia/Almaty", "UTC +6", 21600)},
            {"ANAST", TimeZone("ANAST", "Anadyr Summer Time", "UTC+12:00", "UTC +12", 43200)},
            {"ANAT", TimeZone("ANAT", "Anadyr Time", "Asia/Anadyr", "UTC +12", 43200)},
            {"AQTT", TimeZone("AQTT", "Aqtobe Time", "Asia/Aqtobe", "UTC +5", 18000)},
            {"ART", TimeZone("ART", "Argentina Time", "UTC-03:00", "UTC -3", -10800)},
            {"AWDT", TimeZone("AWDT", "Australian Western Daylight Time", "UTC+09:00", "UTC +9", 32400)},
            {"AWST", TimeZone("AWST", "Australian Western Standard Time", "UTC+08:00", "UTC +8", 28800)},
            {"AZOST", TimeZone("AZOST", "Azores Summer Time", "Atlantic/Azores", "UTC +0", 0)},
            {"AZOT", TimeZone("AZOT", "Azores Time", "UTC-01:00", "UTC -1", -3600)},
            {"AZST", TimeZone("AZST", "Azerbaijan Summer Time", "UTC+05:00", "UTC +5", 18000)},
            {"AZT", TimeZone("AZT", "Azerbaijan Time", "Asia/Baku", "UTC +4", 14400)},
            {"AoE", TimeZone("AoE", "Anywhere on Earth", "UTC-12:00", "UTC -12", -43200)},
            {"B", TimeZone("B", "Bravo Time Zone", "UTC+02:00", "UTC +2", 7200)},
            {"BNT", TimeZone("BNT", "Brunei Darussalam Time", "Asia/Brunei", "UTC +8", 28800)},
            {"BOT", TimeZone("BOT", "Bolivia Time", "America/La_Paz", "UTC -4", -14400)},
            {"BRST", TimeZone("BRST", "Brasília Summer Time", "UTC-02:00", "UTC -2", -7200)},
            {"BRT", TimeZone("BRT", "Brasília Time", "UTC-03:00", "UTC -3", -10800)},
            {"BTT", TimeZone("BTT", "Bhutan Time", "Asia/Thimphu", "UTC +6", 21600)},
            {"C", TimeZone("C", "Charlie Time Zone", "UTC+03:00", "UTC +3", 10800)},
            {"CAST", TimeZone("CAST", "Casey Time", "Antarctica/Casey", "UTC +8", 28800)},
            {"CAT", TimeZone("CAT", "Central Africa Time", "UTC+02:00", "UTC +2", 7200)},
            {"CCT", TimeZone("CCT", "Cocos Islands Time", "Indian/Cocos", "UTC +6:30", 23400)},
            {"CEST", TimeZone("CEST", "Central European Summer Time", "UTC+02:00", "UTC +2", 7200)},
            {"CET", TimeZone("CET", "Central European Time", "UTC+01:00", "UTC +1", 3600)},
            {"CHADT", TimeZone("CHADT", "Chatham Island Daylight Time", "", "UTC +13:45", 49500)},
            {"CHAST", TimeZone("CHAST", "Chatham Island Standard Time", "Pacific/Chatham", "UTC +12:45", 45900)},
            {"CHOST", TimeZone("CHOST", "Choibalsan Summer Time", "UTC+09:00", "UTC +9", 32400)},
            {"CHOT", TimeZone("CHOT", "Choibalsan Time", "Asia/Choibalsan", "UTC +8", 28800)},
            {"CHUT", TimeZone("CHUT", "Chuuk Time", "Pacific/Chuuk", "UTC +10", 36000)},
            {"CIDST", TimeZone("CIDST", "Cayman Islands Daylight Saving Time", "UTC-04:00", "UTC -4", -14400)},
            {"CIST", TimeZone("CIST", "Cayman Islands Standard Time", "America/Cayman", "UTC -5", -18000)},
            {"CKT", TimeZone("CKT", "Cook Island Time", "Pacific/Rarotonga", "UTC -10", -36000)},
            {"CLST", TimeZone("CLST", "Chile Summer Time", "UTC-03:00", "UTC -3", -10800)},
            {"CLT", TimeZone("CLT", "Chile Standard Time", "UTC-04:00", "UTC -4", -14400)},
            {"COT", TimeZone("COT", "Colombia Time", "America/Bogota", "UTC -5", -18000)},
            {"CVT", TimeZone("CVT", "Cape Verde Time", "Atlantic/Cape_Verde", "UTC -1", -3600)},
            {"CXT", TimeZone("CXT", "Christmas Island Time", "Indian/Christmas", "UTC +7", 25200)},
            {"ChST", TimeZone("ChST", "Chamorro Standard Time", "Pacific/Saipan", "UTC +10", 36000)},
            {"D", TimeZone("D", "Delta Time Zone", "UTC+04:00", "UTC +4", 14400)},
            {"DAVT", TimeZone("DAVT", "Davis Time", "Antarctica/Davis", "UTC +7", 25200)},
            {"DDUT", TimeZone("DDUT", "Dumont-d'Urville Time", "UTC+10:00", "UTC +10", 36000)},
            {"E", TimeZone("E", "Echo Time Zone", "UTC+05:00", "UTC +5", 18000)},
            {"EASST", TimeZone("EASST", "Easter Island Summer Time", "UTC-05:00", "UTC -5", -18000)},
            {"EAST", TimeZone("EAST", "Easter Island Standard Time", "UTC-06:00", "UTC -6", -21600)},
            {"EAT", TimeZone("EAT", "Eastern Africa Time", "UTC+03:00", "UTC +3", 10800)},
            {"ECT", TimeZone("ECT", "Ecuador Time", "UTC-05:00", "UTC -5", -18000)},
            {"EDT", TimeZone("EDT", "Eastern Daylight Time", "UTC-04:00", "UTC -4", -14400)},
            {"EEST", TimeZone("EEST", "Eastern European Summer Time", "UTC+03:00", "UTC +3", 10800)},
            {"EET", TimeZone("EET", "Eastern European Time", "UTC+02:00", "UTC +2", 7200)},
            {"EGST", TimeZone("EGST", "Eastern Greenland Summer Time", "UTC+00:00", "UTC +0", 0)},
            {"EGT", TimeZone("EGT", "East Greenland Time", "UTC-01:00", "UTC -1", -3600)},
            {"EST", TimeZone("EST", "Eastern Standard Time", "UTC-05:00", "UTC -5", -18000)},
            {"F", TimeZone("F", "Foxtrot Time Zone", "UTC+06:00", "UTC +6", 21600)},
            {"FET", TimeZone("FET", "Further-Eastern European Time", "UTC+03:00", "UTC +3", 10800)},
            {"FJST", TimeZone("FJST", "Fiji Summer Time", "UTC+13:00", "UTC +13", 46800)},
            {"FJT", TimeZone("FJT", "Fiji Time", "Pacific/Fiji", "UTC +12", 43200)},
            {"FKST", TimeZone("FKST", "Falkland Islands Summer Time", "UTC-03:00", "UTC -3", -10800)},
            {"FKT", TimeZone("FKT", "Falkland Island Time", "UTC-04:00", "UTC -4", -14400)},
            {"FNT", TimeZone("FNT", "Fernando de Noronha Time", "America/Noronha", "UTC -2", -7200)},
            {"G", TimeZone("G", "Golf Time Zone", "UTC+07:00", "UTC +7", 25200)},
            {"GALT", TimeZone("GALT", "Galapagos Time", "Pacific/Galapagos", "UTC -6", -21600)},
            {"GAMT", TimeZone("GAMT", "Gambier Time", "Pacific/Gambier", "UTC -9", -32400)},
            {"GET", TimeZone("GET", "Georgia Standard Time", "Asia/Tbilisi", "UTC +4", 14400)},
            {"GFT", TimeZone("GFT", "French Guiana Time", "America/Cayenne", "UTC -3", -10800)},
            {"GILT", TimeZone("GILT", "Gilbert Island Time", "Pacific/Tarawa", "UTC +12", 43200)},
            {"GMT", TimeZone("GMT", "Greenwich Mean Time", "UTC+00:00", "UTC +0", 0)},
            {"GYT", TimeZone("GYT", "Guyana Time", "America/Guyana", "UTC -4", -14400)},
            {"H", TimeZone("H", "Hotel Time Zone", "UTC+08:00", "UTC +8", 28800)},
            {"HDT", TimeZone("HDT", "Hawaii-Aleutian Daylight Time", "UTC-09:00", "UTC -9", -32400)},
            {"HKT", TimeZone("HKT", "Hong Kong Time", "UTC+08:00", "UTC +8", 28800)},
            {"HOVST", TimeZone("HOVST", "Hovd Summer Time", "UTC+08:00", "UTC +8", 28800)},
            {"HOVT", TimeZone("HOVT", "Hovd Time", "Asia/Hovd", "UTC +7", 25200)},
            {"HST", TimeZone("HST", "Hawaii Standard Time", "Pacific/Honolulu", "UTC -10", -36000)},
            {"I", TimeZone("I", "India Time Zone", "UTC+09:00", "UTC +9", 32400)},
            {"ICT", TimeZone("ICT", "Indochina Time", "UTC+07:00", "UTC +7", 25200)},
            {"IDT", TimeZone("IDT", "Israel Daylight Time", "UTC+03:00", "UTC +3", 10800)},
            {"IOT", TimeZone("IOT", "Indian Chagos Time", "Indian/Chagos", "UTC +6", 21600)},
            {"IRDT", TimeZone("IRDT", "Iran Daylight Time", "Asia/Tehran", "UTC +4:30", 16200)},
            {"IRKST", TimeZone("IRKST", "Irkutsk Summer Time", "UTC+09:00", "UTC +9", 32400)},
            {"IRKT", TimeZone("IRKT", "Irkutsk Time", "Asia/Irkutsk", "UTC +8", 28800)},
            {"IRST", TimeZone("IRST", "Iran Standard Time", "UTC+03:30", "UTC +3:30", 12600)},
            {"JST", TimeZone("JST", "Japan Standard Time", "Asia/Tokyo", "UTC +9", 32400)},
            {"K", TimeZone("K", "Kilo Time Zone", "UTC+10:00", "UTC +10", 36000)},
            {"KGT", TimeZone("KGT", "Kyrgyzstan Time", "Asia/Bishkek", "UTC +6", 21600)},
            {"KOST", TimeZone("KOST", "Kosrae Time", "Pacific/Kosrae", "UTC +11", 39600)},
            {"KRAST", TimeZone("KRAST", "Krasnoyarsk Summer Time", "UTC+08:00", "UTC +8", 28800)},
            {"KRAT", TimeZone("KRAT", "Krasnoyarsk Time", "Asia/Krasnoyarsk", "UTC +7", 25200)},
            {"KST", TimeZone("KST", "Korea Standard Time", "Asia/Pyongyang", "UTC +9", 32400)},
            {"KUYT", TimeZone("KUYT", "Kuybyshev Time", "UTC+04:00", "UTC +4", 14400)},
            {"L", TimeZone("L", "Lima Time Zone", "UTC+11:00", "UTC +11", 39600)},
            {"LHDT", TimeZone("LHDT", "Lord Howe Daylight Time", "UTC+11:00", "UTC +11", 39600)},
            {"LHST", TimeZone("LHST", "Lord Howe Standard Time", "Australia/Lord_Howe", "UTC +10:30", 37800)},
            {"LINT", TimeZone("LINT", "Line Islands Time", "UTC+14:00", "UTC +14", 50400)},
            {"M", TimeZone("M", "Mike Time Zone", "UTC+12:00", "UTC +12", 43200)},
            {"MAGST", TimeZone("MAGST", "Magadan Summer Time", "UTC+12:00", "UTC +12", 43200)},
            {"MAGT", TimeZone("MAGT", "Magadan Time", "Asia/Magadan", "UTC +11", 39600)},
            {"MART", TimeZone("MART", "Marquesas Time", "Pacific/Marquesas", "UTC -9:30", -34200)},
            {"MAWT", TimeZone("MAWT", "Mawson Time", "Antarctica/Mawson", "UTC +5", 18000)},
            {"MDT", TimeZone("MDT", "Mountain Daylight Time", "UTC-06:00", "UTC -6", -21600)},
            {"MHT", TimeZone("MHT", "Marshall Islands Time", "UTC+12:00", "UTC +12", 43200)},
            {"MMT", TimeZone("MMT", "Myanmar Time", "Asia/Yangon", "UTC +6:30", 23400)},
            {"MSD", TimeZone("MSD", "Moscow Daylight Time", "UTC+04:00", "UTC +4", 14400)},
            {"MSK", TimeZone("MSK", "Moscow Standard Time", "Europe/Moscow", "UTC +3", 10800)},
            {"MST", TimeZone("MST", "Mountain Standard Time", "UTC-07:00", "UTC -7", -25200)},
            {"MUT", TimeZone("MUT", "Mauritius Time", "Indian/Mauritius", "UTC +4", 14400)},
            {"MVT", TimeZone("MVT", "Maldives Time", "Indian/Maldives", "UTC +5", 18000)},
            {"MYT", TimeZone("MYT", "Malaysia Time", "Asia/Kuala_Lumpur", "UTC +8", 28800)},
            {"N", TimeZone("N", "November Time Zone", "UTC-01:00", "UTC -1", -3600)},
            {"NCT", TimeZone("NCT", "New Caledonia Time", "Pacific/Noumea", "UTC +11", 39600)},
            {"NDT", TimeZone("NDT", "Newfoundland Daylight Time", "", "UTC -2:30", -9000)},
            {"NFDT", TimeZone("NFDT", "Norfolk Daylight Time", "UTC+12:00", "UTC +12", 43200)},
            {"NFT", TimeZone("NFT", "Norfolk Time", "Pacific/Norfolk", "UTC +11", 39600)},
            {"NOVST", TimeZone("NOVST", "Novosibirsk Summer Time", "UTC+07:00", "UTC +7", 25200)},
            {"NOVT", TimeZone("NOVT", "Novosibirsk Time", "Asia/Novosibirsk", "UTC +7", 25200)},
            {"NPT", TimeZone("NPT", "Nepal Time", "Asia/Kathmandu", "UTC +5:45", 20700)},
            {"NRT", TimeZone("NRT", "Nauru Time", "Pacific/Nauru", "UTC +12", 43200)},
            {"NST", TimeZone("NST", "Newfoundland Standard Time", "UTC-03:30", "UTC -3:30", -12600)},
            {"NUT", TimeZone("NUT", "Niue Time", "Pacific/Niue", "UTC -11", -39600)},
            {"NZDT", TimeZone("NZDT", "New Zealand Daylight Time", "UTC+13:00", "UTC +13", 46800)},
            {"NZST", TimeZone("NZST", "New Zealand Standard Time", "UTC+12:00", "UTC +12", 43200)},
            {"O", TimeZone("O", "Oscar Time Zone", "UTC-02:00", "UTC -2", -7200)},
            {"OMSST", TimeZone("OMSST", "Omsk Summer Time", "UTC+07:00", "UTC +7", 25200)},
            {"OMST", TimeZone("OMST", "Omsk Standard Time", "Asia/Omsk", "UTC +6", 21600)},
            {"ORAT", TimeZone("ORAT", "Oral Time", "Asia/Oral", "UTC +5", 18000)},
            {"P", TimeZone("P", "Papa Time Zone", "UTC-03:00", "UTC -3", -10800)},
            {"PDT", TimeZone("PDT", "Pacific Daylight Time", "UTC-07:00", "UTC -7", -25200)},
            {"PET", TimeZone("PET", "Peru Time", "America/Lima", "UTC -5", -18000)},
            {"PETST", TimeZone("PETST", "Kamchatka Summer Time", "UTC+12:00", "UTC +12", 43200)},
            {"PETT", TimeZone("PETT", "Kamchatka Time", "Asia/Kamchatka", "UTC +12", 43200)},
            {"PGT", TimeZone("PGT", "Papua New Guinea Time", "Pacific/Port_Moresby", "UTC +10", 36000)},
            {"PHOT", TimeZone("PHOT", "Phoenix Island Time", "UTC+13:00", "UTC +13", 46800)},
            {"PHT", TimeZone("PHT", "Philippine Time", "Asia/Manila", "UTC +8", 28800)},
            {"PKT", TimeZone("PKT", "Pakistan Standard Time", "Asia/Karachi", "UTC +5", 18000)},
            {"PMDT", TimeZone("PMDT", "Pierre & Miquelon Daylight Time", "UTC-02:00", "UTC -2", -7200)},
            {"PMST", TimeZone("PMST", "Pierre & Miquelon Standard Time", "UTC-03:00", "UTC -3", -10800)},
            {"PONT", TimeZone("PONT", "Pohnpei Standard Time", "Pacific/Pohnpei", "UTC +11", 39600)},
            {"PWT", TimeZone("PWT", "Palau Time", "Pacific/Palau", "UTC +9", 32400)},
            {"PYST", TimeZone("PYST", "Paraguay Summer Time", "UTC-03:00", "UTC -3", -10800)},
            {"Q", TimeZone("Q", "Quebec Time Zone", "America/Blanc-Sablon", "UTC -4", -14400)},
            {"QYZT", TimeZone("QYZT", "Qyzylorda Time", "UTC+06:00", "UTC +6", 21600)},
            {"R", TimeZone("R", "Romeo Time Zone", "UTC-05:00", "UTC -5", -18000)},
            {"RET", TimeZone("RET", "Reunion Time", "Indian/Reunion", "UTC +4", 14400)},
            {"ROTT", TimeZone("ROTT", "Rothera Time", "Antarctica/Rothera", "UTC -3", -10800)},
            {"S", TimeZone("S", "Sierra Time Zone", "UTC-06:00", "UTC -6", -21600)},
            {"SAKT", TimeZone("SAKT", "Sakhalin Time", "Asia/Sakhalin", "UTC +11", 39600)},
            {"SAMT", TimeZone("SAMT", "Samara Time", "Europe/Samara", "UTC +4", 14400)},
            {"SAST", TimeZone("SAST", "South Africa Standard Time", "UTC+02:00", "UTC +2", 7200)},
            {"SBT", TimeZone("SBT", "Solomon Islands Time", "Pacific/Guadalcanal", "UTC +11", 39600)},
            {"SCT", TimeZone("SCT", "Seychelles Time", "Indian/Mahe", "UTC +4", 14400)},
            {"SGT", TimeZone("SGT", "Singapore Time", "Asia/Singapore", "UTC +8", 28800)},
            {"SRET", TimeZone("SRET", "Srednekolymsk Time", "Asia/Srednekolymsk", "UTC +11", 39600)},
            {"SRT", TimeZone("SRT", "Suriname Time", "America/Paramaribo", "UTC -3", -10800)},
            {"SST", TimeZone("SST", "Samoa Standard Time", "UTC-11:00", "UTC -11", -39600)},
            {"SYOT", TimeZone("SYOT", "Syowa Time", "Antarctica/Syowa", "UTC +3", 10800)},
            {"T", TimeZone("T", "Tango Time Zone", "UTC-07:00", "UTC -7", -25200)},
            {"TAHT", TimeZone("TAHT", "Tahiti Time", "Pacific/Tahiti", "UTC -10", -36000)},
            {"TFT", TimeZone("TFT", "French Southern and Antarctic Time", "UTC+05:00", "UTC +5", 18000)},
            {"TJT", TimeZone("TJT", "Tajikistan Time", "Asia/Dushanbe", "UTC +5", 18000)},
            {"TKT", TimeZone("TKT", "Tokelau Time", "Pacific/Fakaofo", "UTC +13", 46800)},
            {"TLT", TimeZone("TLT", "East Timor Time", "Asia/Dili", "UTC +9", 32400)},
            {"TMT", TimeZone("TMT", "Turkmenistan Time", "Asia/Ashgabat", "UTC +5", 18000)},
            {"TOST", TimeZone("TOST", "Tonga Summer Time", "UTC+14:00", "UTC +14", 50400)},
            {"TOT", TimeZone("TOT", "Tonga Time", "Pacific/Tongatapu", "UTC +13", 46800)},
            {"TRT", TimeZone("TRT", "Turkey Time", "Europe/Istanbul", "UTC +3", 10800)},
            {"TVT", TimeZone("TVT", "Tuvalu Time", "Pacific/Funafuti", "UTC +12", 43200)},
            {"U", TimeZone("U", "Uniform Time Zone", "UTC-08:00", "UTC -8", -28800)},
            {"ULAST", TimeZone("ULAST", "Ulaanbaatar Summer Time", "UTC+09:00", "UTC +9", 32400)},
            {"ULAT", TimeZone("ULAT", "Ulaanbaatar Time", "Asia/Ulaanbaatar", "UTC +8", 28800)},
            {"UYST", TimeZone("UYST", "Uruguay Summer Time", "UTC-02:00", "UTC -2", -7200)},
            {"UYT", TimeZone("UYT", "Uruguay Time", "America/Montevideo", "UTC -3", -10800)},
            {"UZT", TimeZone("UZT", "Uzbekistan Time", "Asia/Tashkent", "UTC +5", 18000)},
            {"V", TimeZone("V", "Victor Time Zone", "UTC-09:00", "UTC -9", -32400)},
            {"VET", TimeZone("VET", "Venezuelan Standard Time", "America/Caracas", "UTC -4", -14400)},
            {"VLAST", TimeZone("VLAST", "Vladivostok Summer Time", "UTC+11:00", "UTC +11", 39600)},
            {"VLAT", TimeZone("VLAT", "Vladivostok Time", "Asia/Vladivostok", "UTC +10", 36000)},
            {"VOST", TimeZone("VOST", "Vostok Time", "Antarctica/Vostok", "UTC +6", 21600)},
            {"VUT", TimeZone("VUT", "Vanuatu Time", "Pacific/Efate", "UTC +11", 39600)},
            {"W", TimeZone("W", "Whiskey Time Zone", "UTC-10:00", "UTC -10", -36000)},
            {"WAKT", TimeZone("WAKT", "Wake Time", "Pacific/Wake", "UTC +12", 43200)},
            {"WARST", TimeZone("WARST", "Western Argentine Summer Time", "UTC-03:00", "UTC -3", -10800)},
            {"WAST", TimeZone("WAST", "West Africa Summer Time", "UTC+02:00", "UTC +2", 7200)},
            {"WAT", TimeZone("WAT", "West Africa Time", "UTC+01:00", "UTC +1", 3600)},
            {"WEST", TimeZone("WEST", "Western European Summer Time", "UTC+01:00", "UTC +1", 3600)},
            {"WET", TimeZone("WET", "Western European Time", "UTC+00:00", "UTC +0", 0)},
            {"WFT", TimeZone("WFT", "Wallis and Futuna Time", "Pacific/Wallis", "UTC +12", 43200)},
            {"WGST", TimeZone("WGST", "Western Greenland Summer Time", "UTC-02:00", "UTC -2", -7200)},
            {"WGT", TimeZone("WGT", "West Greenland Time", "UTC-03:00", "UTC -3", -10800)},
            {"WIB", TimeZone("WIB", "Western Indonesian Time", "UTC+07:00", "UTC +7", 25200)},
            {"WIT", TimeZone("WIT", "Eastern Indonesian Time", "UTC+09:00", "UTC +9", 32400)},
            {"WITA", TimeZone("WITA", "Central Indonesian Time", "UTC+08:00", "UTC +8", 28800)},
            {"WT", TimeZone("WT", "Western Sahara Standard Time", "UTC+00:00", "UTC +0", 0)},
            {"X", TimeZone("X", "X-ray Time Zone", "UTC-11:00", "UTC -11", -39600)},
            {"Y", TimeZone("Y", "Yankee Time Zone", "UTC-12:00", "UTC -12", -43200)},
            {"YAKST", TimeZone("YAKST", "Yakutsk Summer Time", "UTC+10:00", "UTC +10", 36000)},
            {"YAKT", TimeZone("YAKT", "Yakutsk Time", "Asia/Yakutsk", "UTC +9", 32400)},
            {"YAPT", TimeZone("YAPT", "Yap Time", "Pacific/Chuuk", "UTC +10", 36000)},
            {"YEKST", TimeZone("YEKST", "Yekaterinburg Summer Time", "UTC+06:00", "UTC +6", 21600)},
            {"YEKT", TimeZone("YEKT", "Yekaterinburg Time", "Asia/Yekaterinburg", "UTC +5", 18000)},
            {"Z", TimeZone("Z", "Zulu Time Zone", "UTC+00:00", "UTC +0", 0)},
        };
    }
}
