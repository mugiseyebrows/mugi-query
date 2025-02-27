#include "binitemdelegate.h"

#include <QPainter>
#include "varianttobytearray.h"
#include "table.h"

static int intCeil(int n, int d) {
    return (n + d - 1) / d;
}

BinItemDelegate::BinItemDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

static QString padLeft(const QString& s, int w) {
    if (s.size() >= w) {
        return s;
    }
    QString space = " ";
    return space.repeated(w - s.size()) + s;
}

static QString padMid(const QString& s1, const QString& s2, int w) {
    QString space = " ";
    int n = std::max(0, (int) (w - s1.size() - s2.size()));
    return s1 + space.repeated(n) + s2;
}


static QString header(int n, bool rtl = true) {
    QStringList res;
    for(int i=0;i<n;i++) {
        res.append(padMid(QString::number(i*8), QString::number(i * 8 + 7), 8));
    }
    return res.join(" ");
}

static QString reversed(const QString& s) {
    QString r;
    std::copy(s.rbegin(), s.rend(), std::back_inserter(r));
    return r;
}

static QStringList reversed(const QStringList& s) {
    QStringList r;
    std::copy(s.rbegin(), s.rend(), std::back_inserter(r));
    return r;
}

static QString bin(const QByteArray& data, int offset, int len = 4, bool rtl = true) {
    QStringList res;
    for (int i=0;i<len;i++) {
        uchar v = 0;
        if (offset + i < data.size()) {
            v = data[offset + i];
        }
        if (rtl) {
            if (offset + i >= data.size()) {
                break;
            }
        }
        QString s = QString("%1").arg(v, 8, 2, QChar('0'));
        if (rtl) {
            res.append(reversed(s));
        } else {
            res.append(s);
        }
    }
    if (rtl) {
        return res.join(" ");
    } else {
        return reversed(res).join(" ");
    }
}

void BinItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QByteArray data = variantToByteArray(index.data());
    if (data.isEmpty()) {
        return;
    }

    painter->save();
#if 0
    QFont font_ = painter->font();
    qDebug() << font_.family() << font_.pointSize();
#endif

    painter->setClipRect(option.rect);

    QFont font("Liberation Mono", 9, QFont::Normal);
    painter->setFont(font);

    QFontMetrics fm(font);
    int charWidth = fm.horizontalAdvance("00000000") / 8;
    int spacing = charWidth / 2;
    int lineHeight = fm.lineSpacing();

    //qDebug() << fm.horizontalAdvance("00000000");

    int verHeaderWidth = charWidth * QString::number(data.size() * 8).size();

    QRect rect = QRect(option.rect);
    rect.adjust(2,2,-2,-2);

    int lineCount = rect.height() / lineHeight;

    Table table(rect, lineHeight, spacing);

    table.insertColumn(0, verHeaderWidth);
    table.insertColumn(1, verHeaderWidth);

    int availableWidth = table.availableWidth();
    int availableChars = availableWidth / charWidth;
    int octCount = availableChars / 9;

    bool small = octCount < 1 || lineCount < 2;

    if (octCount > data.size()) {
        octCount = data.size();
    }

    if (octCount < 1) {
        octCount = 1;
    }

    int lineCount_ = intCeil(data.size(), octCount) + 1; // lines to display all data + header

    if (lineCount > lineCount_) {
        lineCount = lineCount_;
    }

    table.setRowCount(lineCount);

    table.insertColumn(1, (octCount * 9 - 1) * charWidth);

    bool elide = (lineCount - 1) * octCount < data.size();

    bool rtl = true;

    if (small) {

        int availableChars = rect.width() / charWidth;
        QString text;
        if (data.size() > 1 && availableChars < 10) {
            text = "…";
        } else {
            octCount = availableChars / 9;
            text = bin(data, 0, octCount, rtl);
            if (octCount < data.size()) {
                // elide
                if (text.size() + 2 > availableChars) {
                    text = text.mid(0, text.size() - 9);
                }
                text = text + " …";
            }
        }

        QTextOption opt(Qt::AlignCenter);
        painter->drawText(rect, text, opt);

    } else {

        QTextOption optLeft(Qt::AlignVCenter | Qt::AlignLeft);
        QTextOption optRight(Qt::AlignVCenter | Qt::AlignRight);

        for(int row=0;row<lineCount;row++) {

            if (row > 0) {
                // vertical header
                painter->setPen(Qt::gray);

                QRect rect0 = table.cell(row, 0);
                QRect rect2 = table.cell(row, 2);

                int i0 = (row - 1) * octCount * 8;
                int i2 = row * octCount * 8 - 1;

                painter->drawText(rect0, QString("%1").arg(i0, 0, 10), optRight);
                painter->drawText(rect2, QString("%1").arg(i2, 0, 10), optLeft);
            }

            QRectF rect1 = table.cell(row, 1);

            if (row == 0) {
                // horizontal header
                painter->setPen(Qt::gray);
                painter->drawText(rect1, header(octCount, rtl), optLeft);
            } else {
                // data row
                painter->setPen(Qt::black);
                int offset = (row - 1) * octCount;
                QString text = bin(data, offset, octCount, rtl);
                if (elide && row == (lineCount - 1)) {
                    text = text.mid(0, text.size() - 8) + "…";
                }
                painter->drawText(rect1, text, optLeft);
            }
        }
    }


    painter->restore();
}
