#include "hexitemdelegate.h"
#include <QPainter>
#include <QRectF>
#include "varianttobytearray.h"
#include "table.h"
#include <QDebug>

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

static QString hex(const QByteArray& data, int offset, int len) {
    QStringList res;
    for (int i=0;i<len;i++) {
        if (offset + i >= data.size()) {
            break;
        }
        res.append(QString("%1").arg((uchar) data[offset + i], 2, 16, QChar('0')));
    }
    return res.join(" ");
}

static QString header(int blockCount) {
    int len = blockCount * 8;
    QByteArray res(len, 'x');
    for(int i=0;i<len;i++) {
        res[i] = i;
    }
    return hex(res, 0, len);
}

static int intCeil(int n, int d) {
    return (n + d - 1) / d;
}

void HexItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QByteArray data = variantToByteArray(index.data());
    if (data.isEmpty()) {
        return;
    }

    painter->save();

    QFont font("Liberation Mono", 9, QFont::Normal);

    QFontMetrics fm(font);
    int lineHeight = fm.lineSpacing();
    int charWidth = fm.horizontalAdvance("00000000") / 8;
    int spacing = charWidth / 2;

    painter->setFont(font);
    QRect rect = QRect(option.rect);
    rect.adjust(2,2,-2,-2);

    int lineCount = rect.height() / lineHeight;

    int verHeaderWidth = QString("%1").arg(data.size(), 0, 16).size() * charWidth;

    painter->setClipRect(option.rect);

    Table table(rect, lineHeight, spacing);

    table.insertColumn(0, verHeaderWidth);

    int availableWidth = table.availableWidth();
    int availableChars = availableWidth / charWidth;

    int blockCount = availableChars / (3 * 8);

    bool small = blockCount < 1 || lineCount < 2;

    if (blockCount < 1) {
        blockCount = 1;
    }

    if (blockCount * 8 > data.size()) {
        blockCount = intCeil(data.size(), 8);
    }

    bool elide = true;
    if (blockCount * 8 * (lineCount - 1) >= data.size()) {
        elide = false;
        lineCount = intCeil(data.size(), 8 * blockCount) + 1;
        table.setRowCount(lineCount);
    }

    table.insertColumn(1, (blockCount * 3 * 8 - 1) * charWidth);

    if (small) {

        QTextOption opt(Qt::AlignCenter);
        int availableChars = rect.width() / charWidth;
        int octCount = availableChars / 3;
        QString text = hex(data, 0, octCount);
        if (octCount < data.size()) {
            if (text.size() + 2 > availableChars) {
                text = text.mid(0, text.size() - 3);
            }
            text = text + " …";
        }
        painter->setPen(Qt::black);
        painter->drawText(rect, text, opt);

    } else {

        QTextOption opt(Qt::AlignVCenter | Qt::AlignLeft);
        for(int row=0;row<lineCount;row++) {
            if (row > 0) {
                // vertical header
                QString text = QString("%1").arg((row - 1) * 8 * blockCount, 2, 16, QChar('0'));
                painter->setPen(Qt::gray);
                painter->drawText(table.cell(row, 0), text, opt);
            }
            if (row == 0) {
                // horizontal header
                painter->setPen(Qt::gray);
                QString text = header(blockCount);
                painter->drawText(table.cell(row, 1), text, opt);
            } else {
                // data row
                painter->setPen(Qt::black);
                int offset = (row - 1) * 8 * blockCount;
                int len = 8 * blockCount;
                QString text = hex(data, offset, len);
                if (elide && row == (lineCount - 1)) {
                    text = text.mid(0, text.size() - 2) + "…";
                }
                painter->drawText(table.cell(row, 1), text, opt);
            }
        }
    }

    painter->restore();
}
