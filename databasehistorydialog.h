#ifndef DATABASEHISTORYDIALOG_H
#define DATABASEHISTORYDIALOG_H

#include <QDialog>

namespace Ui {
class DatabaseHistoryDialog;
}

class DatabaseHistoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseHistoryDialog(QWidget *parent = 0);
    ~DatabaseHistoryDialog();

    QString connectionName() const;
    QString driver() const;
    QString host() const;
    QString user() const;
    QString password() const;
    QString database() const;
    int port() const;

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::DatabaseHistoryDialog *ui;
};

#endif // DATABASEHISTORYDIALOG_H
