#include "codewidget.h"
#include "ui_codewidget.h"

#include <QClipboard>
#include <QApplication>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QFont>

CodeWidget::CodeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CodeWidget)
{
    ui->setupUi(this);

    QFont font("Liberation Mono", 11, QFont::Normal);
    ui->code->setFont(font);
}

CodeWidget::~CodeWidget()
{
    delete ui;
}

void CodeWidget::setText(const QString &text)
{
    ui->code->setPlainText(text);
}

void CodeWidget::on_copy_clicked()
{
    qApp->clipboard()->setText(ui->code->toPlainText());
}

void CodeWidget::setHighlighter(QSyntaxHighlighter* highlighter) {
    highlighter->setDocument(ui->code->document());
}
