#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include "schema2export.h"

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = nullptr);
    ~ExportDialog();

    void setName(const QString& name);

    bool clipboard() const;

    bool overwrite() const;

    QString path() const;

    Schema2Export::ExportFormat format() const;

public slots:
    void on_format_clicked(int);

protected:
    QList<Schema2Export::ExportFormat> mFormats;

    Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H
