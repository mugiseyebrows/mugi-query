#include "hexitemdelegate.h"

HexItemDelegate::HexItemDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}


QString HexItemDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    auto t = value.typeId();
    if (t == QMetaType::QByteArray) {
        QByteArray data = value.toByteArray();
        QStringList hex;
        int showSize = qMin(data.size(), 256);
        for (int i=0;i<showSize;i++) {
            hex << QString("%1").arg((uchar) data[i], 2, 16, QChar('0'));
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
