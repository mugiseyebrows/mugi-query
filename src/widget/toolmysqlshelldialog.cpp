#include "toolmysqlshelldialog.h"
#include "ui_toolmysqlshelldialog.h"

ToolMysqlShellDialog::ToolMysqlShellDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolMysqlShellDialog)
{
    ui->setupUi(this);
}

ToolMysqlShellDialog::~ToolMysqlShellDialog()
{
    delete ui;
}

bool ToolMysqlShellDialog::ssl() const
{
    return ui->ssl->isChecked();
}
