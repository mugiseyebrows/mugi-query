#include "itemdelegate.h"

#include <QDateTime>
#include <QDebug>

#include "settings.h"

ItemDelegate::ItemDelegate(QObject *parent) : QStyledItemDelegate (parent)
{

}

QString ItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    auto t = value.typeId();
    if (t == QMetaType::QDateTime || t == QMetaType::QDate || t == QMetaType::QTime) {
        if (Settings::instance()->dateTimeUseLocale()) {
            return QStyledItemDelegate::displayText(value,locale);
        } else {
            if (t == QMetaType::QDateTime) {
                const QDateTime dateTime = value.toDateTime();
                QString text = dateTime.toString(Settings::instance()->dateFormat())
                        + QLatin1Char(' ')
                        + dateTime.toString(Settings::instance()->timeFormat());
                return text;
            } else if (t == QMetaType::QDate) {
                return value.toDate().toString(Settings::instance()->dateFormat());
            } else if (t == QMetaType::QTime) {
                return value.toTime().toString(Settings::instance()->timeFormat());
            }
        }
    } else if (t == QMetaType::Double) {
        if (!Settings::instance()->realUseLocale()) {
            return QString::number(value.toDouble());
        }
    } else if (t == QMetaType::QByteArray) {
        QByteArray data = value.toByteArray();
        /*auto* codec = QTextCodec::codecForName("UTF-8");
        auto* decoder = codec->makeDecoder();
        QString text = decoder->toUnicode(data);
        if (!decoder->hasFailure()) {
            return text;
        }*/

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
    } else if (t == QVariant::List) {
        QVariantList vs = value.toList();
        QVariant v1 = vs.value(0);
        QVariant v2 = vs.value(1);
        return QString("%1 -> %2")
                .arg(v1.isNull() ? QString() : displayText(v1, locale))
                .arg(v2.isNull() ? QString() : displayText(v2, locale));
    }
    return QStyledItemDelegate::displayText(value,locale);
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    
    auto t = index.data().typeId();
    if (t == QMetaType::QDateTime || t == QMetaType::QDate || t == QMetaType::QTime) {
        int w = option.fontMetrics.horizontalAdvance(displayText(index.data(), option.locale)) + 16;
        int h = option.fontMetrics.lineSpacing();
        return QSize(w,h);
    }
    return QStyledItemDelegate::sizeHint(option,index);
}

