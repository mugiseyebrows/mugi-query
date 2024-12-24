#include "hexitemdelegate.h"
#include <QPainter>
#include <QRectF>

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

static QString hex(const QByteArray& data, int offset, int len = 16) {
    QStringList res;
    for (int i=0;i<len;i++) {
        if (offset + i >= data.size()) {
            break;
        }
        res.append(QString("%1").arg((uchar) data[offset + i], 2, 16, QChar('0')));
    }
    return res.join(" ");
}

void HexItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    QFont font("Courier New", 8, QFont::Normal);

    QFontMetrics fm(font);
    //double rw = fm.horizontalAdvance("000");
    double lineHeight = fm.lineSpacing();

    painter->setFont(font);
    QRectF rect = QRectF(option.rect);
    rect.adjust(2,2,-2,-2);

    int lineCount = rect.height() / lineHeight;

    QByteArray data = index.data().toByteArray();

    painter->setClipRect(option.rect);

    double dataWidth = fm.horizontalAdvance(hex(QByteArray(16, 'x'), 0)) + 5;
    double rulerWidth = fm.horizontalAdvance("00") + 5;
    double spacing = fm.horizontalAdvance("0") / 2;

    if (rect.width() < (dataWidth + rulerWidth + spacing) || lineCount < 2) {

        int chars = rect.width() / fm.horizontalAdvance("0");
        int octs = (chars - 2) / 3 + 1;

        bool elide = octs * lineCount < data.size();

        double y0 = (rect.height() - lineHeight * lineCount) / 2;
        double x0 = 0;

        QTextOption opt(Qt::AlignVCenter | Qt::AlignLeft);

        for(int i=0;i<lineCount;i++) {
            QString text;
            if (elide && i == lineCount - 1) {
                text = hex(data, i * octs, octs - 1) + " …";
            } else {
                text = hex(data, i * octs, octs);
            }
            QRectF rect1(rect.topLeft() + QPointF(x0, y0 + i * lineHeight), QSizeF(rect.width(), lineHeight));
            painter->drawText(rect1, text, opt);
        }


    } else {

        double x0 = (rect.width() - (dataWidth + rulerWidth + spacing)) / 2;
        double x1 = x0 + rulerWidth + spacing;

        double y0 = (rect.height() - lineHeight * lineCount) / 2;

        double h = lineHeight;

        QByteArray header(16, 'x');
        for(int i=0;i<16;i++) {
            header[i] = i;
        }

        QTextOption opt(Qt::AlignVCenter | Qt::AlignLeft);


        for(int i=0;i<lineCount;i++) {

            if (i > 0) {
                QRectF rect1(rect.topLeft() + QPointF(x0, y0 + i * lineHeight), QSizeF(rulerWidth, h));
                painter->setPen(Qt::gray);
                painter->drawText(rect1, QString("%1").arg((i - 1) * 16, 2, 16, QChar('0')), opt);
            }

            QRectF rect2(rect.topLeft() + QPointF(x1, y0 + i * lineHeight), QSizeF(dataWidth, h));
            if (i == 0) {
                painter->setPen(Qt::gray);
                painter->drawText(rect2, hex(header, 0), opt);
            } else {
                painter->setPen(Qt::black);
                painter->drawText(rect2, hex(data, (i - 1) * 16), opt);
            }


        }


    }


    painter->restore();

    //painter->drawEllipse(option.rect.topLeft() + QPoint(10,10), 4, 4);
    //qDebug() << "rect" << option.rect;
}
