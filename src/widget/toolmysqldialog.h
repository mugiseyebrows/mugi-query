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

    explicit ToolMysqlDialog(QWidget *parent = nullptr);
    ~ToolMysqlDialog();

    QStringList files() const;
    bool ssl() const;

protected:
    QStringListModel* mFiles;

    QString mDir;

private slots:
    void on_addFile_clicked();

    void on_addDirectory_clicked();

private:
    Ui::ToolMysqlDialog *ui;
};

#endif // TOOLMYSQLDIALOG_H
