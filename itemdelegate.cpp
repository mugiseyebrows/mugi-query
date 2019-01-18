#include "itemdelegate.h"

#include <QDateTime>
#include <QDebug>

#include "settings.h"

ItemDelegate::ItemDelegate(QObject *parent) : QStyledItemDelegate (parent)
{

}

QString ItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    QVariant::Type t = value.type();
    if (t == QVariant::DateTime || t == QVariant::Date || t == QVariant::Time) {
        if (Settings::instance()->dateTimeUseLocale()) {
            return QStyledItemDelegate::displayText(value,locale);
        } else {
            if (t == QVariant::DateTime) {
                const QDateTime dateTime = value.toDateTime();
                QString text = dateTime.toString(Settings::instance()->dateFormat())
                        + QLatin1Char(' ')
                        + dateTime.toString(Settings::instance()->timeFormat());
                return text;
            } else if (t == QVariant::Date) {
                return value.toDate().toString(Settings::instance()->dateFormat());
            } else if (t == QVariant::Time) {
                return value.toTime().toString(Settings::instance()->timeFormat());
            }
        }
    } else if (t == QVariant::Double) {
        if (!Settings::instance()->realUseLocale()) {
            return QString::number(value.toDouble());
        }
    } else if (t == QVariant::ByteArray) {
        QByteArray data = value.toByteArray();
        QStringList hex;
        int showSize = qMin(data.size(),256);
        for (int i=0;i<showSize;i++) {
            //hex << QString::number((uchar) data[i], 16).rightJustified(2,'0');
            hex << QString("%1").arg((uchar) data[i],2,16,QChar('0'));
            if (i % 16 == 15)
                hex << "\n";
        }
        if (data.size() > showSize) {
            hex << "...";
        }
        return hex.join(" ");
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

