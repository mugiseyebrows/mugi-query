#ifndef COMPLETERITEMDELEGATE_H
#define COMPLETERITEMDELEGATE_H

#include <QItemDelegate>

class CompleterItemDelegate : public QItemDelegate {
    Q_OBJECT
public:
    CompleterItemDelegate(QAbstractItemView *view);

    //void paint(QPainter *p, const QStyleOptionViewItem& opt, const QModelIndex& idx) const override;

    void setPrefix(const QString& value);

protected:
    void drawDisplay(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QString &text) const;
private:
    QAbstractItemView *view;
    QString mPrefix;
};


#endif // COMPLETERITEMDELEGATE_H
