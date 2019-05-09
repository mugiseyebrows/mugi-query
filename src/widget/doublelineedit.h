#ifndef DOUBLELINEEDIT_H
#define DOUBLELINEEDIT_H

#include <QLineEdit>

class DoubleLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
public:
    explicit DoubleLineEdit(QWidget *parent = 0, int prec = 6);
    
    double value(bool* ok = 0) const;

    void setValue(double value, int prec = -1);

    void setPrec(int value);

    int prec() const;

    void setIfNoValue(double value, int prec = -1);
protected:
    int mPrec;

signals:
    void valueChanged(double);
public slots:
    
    void onTextChanged(QString val);
};

#endif // DOUBLELINEEDIT_H
