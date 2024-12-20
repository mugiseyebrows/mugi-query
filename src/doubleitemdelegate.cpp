#include "doubleitemdelegate.h"

#include "settings.h"

DoubleItemDelegate::DoubleItemDelegate(QObject *parent, int prec) : QStyledItemDelegate(parent), mPrec(prec)
{

}

QString DoubleItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    auto t = value.typeId();
    if (t == QMetaType::Double) {
        if (!Settings::instance()->realUseLocale()) {
            return QString::number(value.toDouble(), 'f', mPrec);
        }
        return locale.toString(value.toDouble(), 'f', mPrec);
    }
    return QStyledItemDelegate::displayText(value,locale);
}

void DoubleItemDelegate::setPrec(int prec)
{
    mPrec = prec;
}
