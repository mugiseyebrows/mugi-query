#ifndef CODEWIDGETDIALOG_H
#define CODEWIDGETDIALOG_H

#include <QDialog>
class QSyntaxHighlighter;

namespace Ui {
class CodeWidgetDialog;
}

class CodeWidgetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CodeWidgetDialog(QWidget *parent = nullptr);
    ~CodeWidgetDialog();

    void setText(const QString& text);

    void setHighlighter(QSyntaxHighlighter* highlighter);

private:
    Ui::CodeWidgetDialog *ui;
};

#endif // CODEWIDGETDIALOG_H
