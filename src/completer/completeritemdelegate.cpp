#include "completeritemdelegate.h"

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QAbstractItemView>
#include <QDebug>
#include <QApplication>

CompleterItemDelegate::CompleterItemDelegate(QAbstractItemView *view) : QItemDelegate(view), view(view) {

}

void CompleterItemDelegate::setPrefix(const QString &value) {
    mPrefix = value;
}

#if 0
void CompleterItemDelegate::paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &idx) const {
    QStyleOptionViewItem optCopy = opt;
    //optCopy.showDecorationSelected = true;
    if (view->currentIndex() == idx)
        optCopy.state |= QStyle::State_HasFocus;

    QItemDelegate::paint(p, optCopy, idx);
}
#endif

static QList<QPair<int,int>> toRanges(const QList<bool>& mask) {
    QList<QPair<int,int>> res;
    if (mask.isEmpty()) {
        return res;
    }
    int p = 0;
    bool v = mask[p];
    for(int i=0;i<mask.size();i++) {
        if (v != mask[i]) {
            res.append({p, i-1});
            p = i;
            v = mask[p];
        }
    }
    res.append({p, mask.size()-1});
    return res;
}

static QStringList split(const QString& s, const QString& ss) {
    QList<bool> mask;
    if (!ss.isEmpty()) {
        int j = 0;
        for(int i=0;i<s.size();i++) {
            if (s[i].toLower() == ss[j].toLower()) {
                mask.append(true);
                j++;
                if (j >= ss.size()) {
                    break;
                }
            } else {
                mask.append(false);
            }
        }
    }
    while(mask.size() < s.size()) {
        mask.append(false);
    }
    QList<QPair<int,int>> ranges = toRanges(mask);
    QStringList res;
    for(const QPair<int,int>& range: ranges) {
        QString s1 = s.mid(range.first, range.second - range.first + 1);
        res.append(s1);
    }
    return res;
}

static int parsum(const QList<int>& vs, int n) {
    int res = 0;
    if (n > vs.size()) {
        n = vs.size();
    }
    for(int i=0;i<n;i++) {
        res += vs[i];
    }
    return res;
}

void CompleterItemDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
                                const QRect &rect, const QString &text) const {

    //if (option.state != QStyle::State_HasFocus)

    QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
                              ? QPalette::Normal : QPalette::Disabled;
    if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
        cg = QPalette::Inactive;
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(rect, option.palette.brush(cg, QPalette::Highlight));
        painter->setPen(option.palette.color(cg, QPalette::HighlightedText));
    } else {
        painter->setPen(option.palette.color(cg, QPalette::Text));
    }

    const QWidget* widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, nullptr, widget) + 1;
    QRect textRect = rect.adjusted(textMargin, 0, -textMargin, 0); // remove width padding

    Qt::Alignment alignment = Qt::AlignLeft | Qt::AlignVCenter;

    if (option.state & QStyle::State_Selected) {
        const QFont& font1 = option.font;
        QFont font2(font1.family(), font1.pointSize(), QFont::Bold);
        QFontMetrics metrics1(font1);
        QFontMetrics metrics2(font2);
        QStringList text2 = split(text, mPrefix);
        QList<int> horizontalAdvance;
        for(int i=0;i<text2.size();i++) {
            if (i % 2) {
                horizontalAdvance.append(metrics1.horizontalAdvance(text2[i]) + 1);
            } else {
                horizontalAdvance.append(metrics2.horizontalAdvance(text2[i]) + 1);
            }
        }
        for(int i=0;i<text2.size();i++) {
            QRect textRect2(textRect.topLeft() + QPoint(parsum(horizontalAdvance, i), 0), QSize(horizontalAdvance[i], textRect.height()));
            if (i % 2) {
                painter->setFont(font1);
            } else {
                painter->setFont(font2);
            }
            painter->drawText(textRect2, alignment, text2[i]);
        }
    } else {
        painter->drawText(textRect, alignment, text);
    }

}
