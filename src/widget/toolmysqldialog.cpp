#include "toolmysqldialog.h"
#include "ui_toolmysqldialog.h"

ToolMysqlDialog::ToolMysqlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolMysqlDialog)
{
    ui->setupUi(this);
    ui->input->setMode(LineSelect::ModeFileOpen);
    ui->input->setFilter("Sql files (*.sql);;All files (*.*)");
}

ToolMysqlDialog::~ToolMysqlDialog()
{
    delete ui;
}

QString ToolMysqlDialog::input() const
{
    return ui->input->text();
}
