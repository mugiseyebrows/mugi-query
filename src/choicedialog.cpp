#include "choicedialog.h"
#include "ui_choicedialog.h"
#include <QRadioButton>

ChoiceDialog::ChoiceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChoiceDialog)
{
    ui->setupUi(this);
}

ChoiceDialog::~ChoiceDialog()
{
    delete ui;
}



void ChoiceDialog::init(const QStringList &items, int checkedIndex)
{
    for(int i=0;i<items.size();i++) {
        auto item = items[i];
        auto* button = new QRadioButton(item);
        button->setChecked(i == checkedIndex);
        mButtons.append(button);
        ui->verticalLayout->addWidget(button);
    }
}

int ChoiceDialog::checkedIndex() const
{
    for(int i=0;i<mButtons.size();i++) {
        if (mButtons[i]->isChecked()) {
            return i;
        }
    }
    return -1;
}
