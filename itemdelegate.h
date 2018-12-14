#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>

class ItemDelegate : public QStyledItemDelegate
{
public:
    ItemDelegate(QObject *parent = Q_NULLPTR);

    QString displayText(const QVariant &value, const QLocale &locale) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};

#endif // ITEMDELEGATE_H
