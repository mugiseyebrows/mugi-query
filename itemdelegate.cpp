#include "itemdelegate.h"

#include <QDateTime>
#include <QDebug>

#include "settings.h"

ItemDelegate::ItemDelegate(QObject *parent) : QStyledItemDelegate (parent)
{

}

QString ItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    if (value.type() == QVariant::DateTime) {
        Settings::DateTimeFormat format = Settings::instance()->dateTimeFormat();
        if (format == Settings::DateTimeFormatWithSeconds) {
            return value.toDateTime().toString("yyyy-MM-dd hh:mm:ss");
        } else if (format == Settings::DateTimeFormatWitoutSeconds) {
            return value.toDateTime().toString("yyyy-MM-dd hh:mm");
        }
    }
    return QStyledItemDelegate::displayText(value,locale);
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.model()->data(index).type() == QVariant::DateTime) {
        qDebug() << "1";
    }
    return QStyledItemDelegate::sizeHint(option,index);
}
