#ifndef TOOLMYSQLDUMPDIALOG_H
#define TOOLMYSQLDUMPDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class ToolMysqldumpDialog;
}

class QSortFilterProxyModel;
class QStandardItemModel;

class ToolMysqldumpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolMysqldumpDialog(QSqlDatabase db, QWidget *parent = nullptr);
    ~ToolMysqldumpDialog();

    QString output() const;

    bool data() const;

    bool schema() const;

    bool multipleFiles() const;

    QStringList tables() const;

private slots:
    void on_multipleFiles_clicked(bool checked);

    void on_checkAll_clicked();

    void on_uncheckAll_clicked();

    void on_filter_textChanged(const QString &arg1);

protected:
    Ui::ToolMysqldumpDialog *ui;
    QSortFilterProxyModel* mProxyModel;
    QStandardItemModel* mModel;
};

#endif // TOOLMYSQLDUMPDIALOG_H
