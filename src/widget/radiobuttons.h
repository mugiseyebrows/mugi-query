#ifndef RADIOBUTTONS_H
#define RADIOBUTTONS_H

#include <QWidget>

class QRadioButton;

namespace Ui {
class RadioButtons;
}

class RadioButtons : public QWidget
{
    Q_OBJECT

public:
    explicit RadioButtons(QWidget *parent = nullptr);
    ~RadioButtons();

    void init(const QStringList& items, int checkedIndex =-1);
    int checkedIndex() const;
    void setChecked(int index);
signals:
    void clicked(int);
protected:
    QList<QRadioButton*> mButtons;
    QStringList mItems;
private:
    Ui::RadioButtons *ui;
};

#endif // RADIOBUTTONS_H
