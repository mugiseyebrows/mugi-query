#ifndef TOOLMYSQLDIALOG_H
#define TOOLMYSQLDIALOG_H

#include <QDialog>

namespace Ui {
class ToolMysqlDialog;
}

class ToolMysqlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolMysqlDialog(QWidget *parent = nullptr);
    ~ToolMysqlDialog();

    QString input() const;

private:
    Ui::ToolMysqlDialog *ui;
};

#endif // TOOLMYSQLDIALOG_H
