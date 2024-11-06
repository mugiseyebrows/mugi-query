#ifndef TOOLMYSQLDUMPDIALOG_H
#define TOOLMYSQLDUMPDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class ToolMysqldumpDialog;
}

class QSortFilterProxyModel;
class CheckableModel;

class MysqldumpSettings {
public:
    enum Path {
        DatabaseName,
        DatabaseDatetimeName
    };
    enum Format {
        OneFile,
        MultipleFiles
    };
    QString output;
    QStringList tables;
    Format format;
    Path path;
    bool schema;
    bool data;
    bool ssl;
    bool tab;
};

class ToolMysqldumpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolMysqldumpDialog(QSqlDatabase db, QWidget *parent = nullptr);
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
};

#endif // TOOLMYSQLDUMPDIALOG_H
