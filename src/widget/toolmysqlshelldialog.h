#ifndef TOOLMYSQLSHELLDIALOG_H
#define TOOLMYSQLSHELLDIALOG_H

#include <QDialog>

namespace Ui {
class ToolMysqlShellDialog;
}

class ToolMysqlShellDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolMysqlShellDialog(QWidget *parent = nullptr);
    ~ToolMysqlShellDialog();

    bool ssl() const;

private:
    Ui::ToolMysqlShellDialog *ui;
};

#endif // TOOLMYSQLSHELLDIALOG_H
