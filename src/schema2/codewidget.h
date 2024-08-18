#ifndef CODEWIDGET_H
#define CODEWIDGET_H

#include <QWidget>
class QSyntaxHighlighter;

namespace Ui {
class CodeWidget;
}

class CodeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CodeWidget(QWidget *parent = nullptr);
    ~CodeWidget();

    void setText(const QString& text);

    void setHighlighter(QSyntaxHighlighter* highlighter);

private slots:
    void on_copy_clicked();

private:
    Ui::CodeWidget *ui;
};

#endif // CODEWIDGET_H
