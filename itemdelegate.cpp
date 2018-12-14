#include "itemdelegate.h"

#include <QDateTime>
#include <QDebug>

#include "settings.h"

ItemDelegate::ItemDelegate(QObject *parent) : QStyledItemDelegate (parent)
{
    mLocale = QLocale(QLocale::C);
}

QString ItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    //return QStyledItemDelegate::displayText(value,locale);
    QVariant::Type t = value.type();
    if (t == QVariant::DateTime || t == QVariant::Date || t == QVariant::Time) {
        DisplayFormat::Locale locale_ = Settings::instance()->dateTimeLocale();
        if (locale_ == DisplayFormat::C || locale_ == DisplayFormat::Current) {
            const QLocale& locale__ = locale_ == DisplayFormat::C ? mLocale : locale;
            QLocale::FormatType format = QLocale::ShortFormat;
            if (t == QVariant::DateTime) {
                const QDateTime dateTime = value.toDateTime();
                QString text = locale__.toString(dateTime.date(), format)
                        + QLatin1Char(' ')
                        + locale__.toString(dateTime.time(), format);
                return text;
            } else if (t == QVariant::Date) {
                return locale__.toString(value.toDate(),format);
            } else if (t == QVariant::Time) {
                return locale__.toString(value.toTime(),format);
            }
        } else if (locale_ == DisplayFormat::Special) {
            if (t == QVariant::DateTime) {
                return value.toDateTime().toString(Settings::instance()->dateTimeFormat());
            } else if (t == QVariant::Date) {
                return value.toDate().toString(Settings::instance()->dateFormat());
            } else if (t == QVariant::Time) {
                return value.toTime().toString(Settings::instance()->timeFormat());
            }
        }
    } else if (t == QVariant::Double || t == QMetaType::Float) {
        DisplayFormat::Locale locale_ = Settings::instance()->realLocale();
        if (locale_ == DisplayFormat::C || locale_ == DisplayFormat::Current) {
            const QLocale& locale__ = locale_ == DisplayFormat::C ? mLocale : locale;
            return locale__.toString(value.toDouble());
        }
    }
    return QStyledItemDelegate::displayText(value,locale);
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QVariant::Type t = index.data().type();
    if (t == QVariant::DateTime || t == QVariant::Date || t == QVariant::Time) {
        int w = option.fontMetrics.width(displayText(index.data(),option.locale)) + 16;
        int h = option.fontMetrics.lineSpacing();
        return QSize(w,h);
    }
    return QStyledItemDelegate::sizeHint(option,index);
}

