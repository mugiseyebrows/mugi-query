#include "confirmationdialog.h"
#include "ui_confirmationdialog.h"

ConfirmationDialog::ConfirmationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfirmationDialog)
{
    ui->setupUi(this);
}

ConfirmationDialog::~ConfirmationDialog()
{
    delete ui;
}

void ConfirmationDialog::setMessage(const QString &message)
{
    ui->message->setText(message);
}

bool ConfirmationDialog::dontAskAgain() const {
    return ui->dontAskAgain->isChecked();
}

bool ConfirmationDialog::question(QWidget *parent, const QString &message, bool* dontAsk)
{
    if (*dontAsk) {
        return true;
    }
    ConfirmationDialog dialog(parent);
    dialog.setMessage(message);
    if (dialog.exec() == QDialog::Accepted) {
        *dontAsk = dialog.dontAskAgain();
        return true;
    }
    return false;
}
