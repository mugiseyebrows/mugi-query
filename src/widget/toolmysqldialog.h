#ifndef TOOLMYSQLDIALOG_H
#define TOOLMYSQLDIALOG_H

#include <QDialog>
class QStringListModel;

namespace Ui {
class ToolMysqlDialog;
}

class ToolMysqlDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        OneFile,
        MultipleFiles,
    };

    explicit ToolMysqlDialog(Mode mode, QWidget *parent = nullptr);
    ~ToolMysqlDialog();

    QStringList inputs() const;

    void setMode(Mode mode);

protected:
    QStringListModel* mModel;
    Mode mMode;

private slots:
    void on_addFile_clicked();

    void on_addDirectory_clicked();

private:
    Ui::ToolMysqlDialog *ui;
};

#endif // TOOLMYSQLDIALOG_H
