#include "codewidgetdialog.h"
#include "ui_codewidgetdialog.h"

CodeWidgetDialog::CodeWidgetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CodeWidgetDialog)
{
    ui->setupUi(this);
}

CodeWidgetDialog::~CodeWidgetDialog()
{
    delete ui;
}

void CodeWidgetDialog::setText(const QString &text)
{
    ui->codeWidget->setText(text);
}

void CodeWidgetDialog::setHighlighter(QSyntaxHighlighter *highlighter)
{
    ui->codeWidget->setHighlighter(highlighter);
}
