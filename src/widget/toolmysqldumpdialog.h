#ifndef TOOLMYSQLDUMPDIALOG_H
#define TOOLMYSQLDUMPDIALOG_H

#include <QDialog>
#include <QSqlDatabase>
#include "mysqldumpsettings.h"

namespace Ui {
class ToolMysqldumpDialog;
}

class QSortFilterProxyModel;
class CheckableModel;

// --routines

class ToolMysqldumpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolMysqldumpDialog(QSqlDatabase db, const MysqldumpSettings& settings, QWidget *parent = nullptr);
    ~ToolMysqldumpDialog();

    MysqldumpSettings settings() const;

#if 0
    QString output() const;

    bool data() const;

    bool schema() const;

    bool multipleFiles() const;

    QStringList tables() const;

    bool tab() const;

    bool ssl() const;

private slots:
    void on_multipleFiles_clicked(bool checked);


    void on_checkAll_clicked();

    void on_uncheckAll_clicked();

    void on_filter_textChanged(const QString &arg1);
#endif

protected:
    Ui::ToolMysqldumpDialog *ui;
#if 0
    QSortFilterProxyModel* mProxyModel;
    CheckableModel* mModel;
#endif

    // QDialog interface
public slots:
    void accept();
private slots:
    void on_multipleFiles_clicked();
    void on_oneFile_clicked();
    void on_help_clicked();
};

#endif // TOOLMYSQLDUMPDIALOG_H
