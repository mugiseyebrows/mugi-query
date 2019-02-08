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
    enum cols {
        col_date,
        col_connectionName,
        col_driver,
        col_host,
        col_user,
        col_password,
        col_database,
        col_port
    };

    explicit DatabaseHistoryDialog(QWidget *parent = 0);
    ~DatabaseHistoryDialog();

    QVariant data(int column) const;
    QString dataToString(int column) const;
    int dataToInt(int column) const;

    QString connectionName() const;
    QString driver() const;
    QString host() const;
    QString user() const;
    QString password() const;
    QString database() const;
    int port() const;
    void refresh();

    void select(const QString& connectionName);

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

private:
    Ui::DatabaseHistoryDialog *ui;
};

#endif // DATABASEHISTORYDIALOG_H
