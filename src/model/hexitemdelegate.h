#ifndef HEXITEMDELEGATE_H
#define HEXITEMDELEGATE_H

#include <QStyledItemDelegate>

class HexItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit HexItemDelegate(QObject *parent = nullptr);

signals:

    // QStyledItemDelegate interface
public:
    QString displayText(const QVariant &value, const QLocale &locale) const override;

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // HEXITEMDELEGATE_H
