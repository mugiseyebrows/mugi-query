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

    QString path() const;

    Schema2Export::ExportFormat format() const;

    bool itemsSelected() const;
    bool itemsAll() const;
    bool cropViewport() const;
    bool cropAll() const;

    void setFormat(int index);
    void setItemsSelected(bool value);
    void setItemsAll(bool value);
    void setCropViewport(bool value);
    void setCropAll(bool value);
    void setClipboard(bool value);
    void setPath(const QString& path);

    void updateExt();
public slots:
    void on_format_clicked(int);

protected:
    QList<Schema2Export::ExportFormat> mFormats;

    Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H
