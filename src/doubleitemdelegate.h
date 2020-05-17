#ifndef DOUBLEITEMDELEGATE_H
#define DOUBLEITEMDELEGATE_H

#include <QStyledItemDelegate>

class DoubleItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    DoubleItemDelegate(QObject *parent = nullptr, int prec = 6);

    QString displayText(const QVariant &value, const QLocale &locale) const;

    void setPrec(int prec);

protected:
    int mPrec;
};

#endif // DOUBLEITEMDELEGATE_H
