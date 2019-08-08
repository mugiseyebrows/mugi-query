#ifndef SAVEDATADIALOG_H
#define SAVEDATADIALOG_H

#include <QDialog>

namespace Ui {
class SaveDataDialog;
}

class QSqlQueryModel;
class DataImportColumnModel;
class CallOnce;

#include "dataformat.h"
#include "outputtype.h"
#include "datasavedialogstate.h"
#include <QSqlDatabase>


class Tokens;

class SaveDataDialog : public QDialog
{
    Q_OBJECT

public:

    explicit SaveDataDialog(const QSqlDatabase &database, QSqlQueryModel *model,
                            const Tokens& tokens, QWidget *parent = 0);
    ~SaveDataDialog();

    DataFormat::Format format() const;

    QList<bool> keysChecked() const;
    QList<bool> dataChecked() const;

    QString table() const;

    OutputType::Type output() const;

    DataImportColumnModel *dataModel() const;
    DataImportColumnModel *keysModel() const;

    void init();

    void save();

    static DataSaveDialogState mState;

    QString filePath() const;

public slots:

    void accept();

private slots:

    void on_table_textChanged(const QString &arg1);

    void on_format_currentIndexChanged(int index);

    void on_output_currentIndexChanged(int index);

    void on_selectPath_clicked();

    void onUpdatePreview();

private:
    Ui::SaveDataDialog *ui;
    QSqlQueryModel* mModel;
    QSqlDatabase mDatabase;
    CallOnce* mUpdatePreview;
};

#endif // SAVEDATADIALOG_H
