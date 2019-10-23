#ifndef DATABASEHISTORYDIALOG_H
#define DATABASEHISTORYDIALOG_H

#include <QDialog>

namespace Ui {
class DatabaseHistoryDialog;
}

class DateTimeRangeWidget;
class QLineEdit;
class QComboBox;
class IntLineEdit;

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
    QString database_() const;
    int port() const;

    void select(const QString& connectionName);

public slots:
    void onUpdateQuery();

private slots:
    void on_tableView_doubleClicked(const QModelIndex &index);

protected:
    Ui::DatabaseHistoryDialog *ui;
    DateTimeRangeWidget* mDateEdit;
    QLineEdit* mConnectionNameEdit;
    QComboBox* mDriverEdit;
    QLineEdit* mHostEdit;
    QLineEdit* mUserEdit;
    QLineEdit* mDatabaseEdit;
    IntLineEdit* mPortEdit;
};


#endif // DATABASEHISTORYDIALOG_H
