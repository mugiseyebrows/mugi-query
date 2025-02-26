#ifndef BINITEMDELEGATE_H
#define BINITEMDELEGATE_H

#include <QStyledItemDelegate>

class BinItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BinItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // BINITEMDELEGATE_H
