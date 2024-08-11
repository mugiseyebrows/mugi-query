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
};

#endif // HEXITEMDELEGATE_H
