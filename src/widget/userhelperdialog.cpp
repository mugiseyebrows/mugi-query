#include "userhelperdialog.h"
#include "ui_userhelperdialog.h"

#include "tokens.h"

UserHelperDialog::UserHelperDialog(const QString& database, const Tokens& tokens, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserHelperDialog)
{
    ui->setupUi(this);
    ui->query->setTokens(tokens);
    ui->database->setText(database);
}

UserHelperDialog::~UserHelperDialog()
{
    delete ui;
}
