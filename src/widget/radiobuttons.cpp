#include "radiobuttons.h"
#include "ui_radiobuttons.h"

#include <QRadioButton>

RadioButtons::RadioButtons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RadioButtons)
{
    ui->setupUi(this);
}

RadioButtons::~RadioButtons()
{
    delete ui;
}

void RadioButtons::init(const QStringList &items, int checkedIndex)
{
    mItems = items;
    for(int i=0;i<items.size();i++) {
        auto item = items[i];
        auto* button = new QRadioButton(item);
        if (i == checkedIndex) {
            button->setChecked(true);
        }
        mButtons.append(button);
        connect(button, qOverload<bool>(QRadioButton::clicked),[=](bool on){
            if (on) {
                emit clicked(i);
            }
        });
        layout()->addWidget(button);
    }
}

int RadioButtons::checkedIndex() const
{
    for(int i=0;i<mButtons.size();i++) {
        if (mButtons[i]->isChecked()) {
            return i;
        }
    }
    return -1;
}
