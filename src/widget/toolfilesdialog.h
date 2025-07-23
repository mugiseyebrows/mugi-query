#ifndef TOOLFILESDIALOG_H
#define TOOLFILESDIALOG_H

#include <QDialog>
#include "enums.h"

namespace Ui {
class ToolFilesDialog;
}

class QStringListModel;

class ToolFilesDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        Mysql,
        PushCsv
    };

    explicit ToolFilesDialog(Mode mode, QWidget *parent = nullptr);
    ~ToolFilesDialog();

    QStringList files() const;
    bool ssl() const;
    bool force() const;
    QString table() const;

    IfExists ifExists() const;

    void append(const QStringList& names);

protected:
    Ui::ToolFilesDialog *ui;
    QStringListModel* mFiles;
    QString mDir;
    Mode mMode;

protected slots:
    void on_addFile_clicked();
    void on_addDirectory_clicked();
};

#endif // TOOLFILESDIALOG_H
