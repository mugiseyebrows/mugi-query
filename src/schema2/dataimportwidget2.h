#ifndef DATAIMPORTWIDGET2_H
#define DATAIMPORTWIDGET2_H

#include <QWidget>
class Schema2TableModel;
class ModelAppender;
class DataImportModel;
class QAbstractItemModel;
class RichHeaderView;
class Schema2Data;
class QSqlRecord;
class CopyEventFilter;
#include <QSqlDatabase>

namespace Ui {
class DataImportWidget2;
}

class DataImportWidget2 : public QWidget
{
    Q_OBJECT

public:
    explicit DataImportWidget2(QWidget *parent = nullptr);
    ~DataImportWidget2();

    void init(Schema2Data *data, Schema2TableModel* table);

protected:
    Schema2TableModel* mTable;
    Ui::DataImportWidget2 *ui;
    ModelAppender*  mAppender;
    DataImportModel* mModel;
    RichHeaderView* mHeaderView;
    Schema2Data* mData;
    CopyEventFilter* mFilter;

    void updateHorizontalHeader();
    void createHorizontalHeader();
    void initCopyFilter();
    void initAppender();
    void initImportModel();
    QSqlRecord record(QSqlDatabase db, const QString &tableName, int row, bool *ok);
protected slots:
    void onDataPaste();
    void onDataCopy();
private slots:
    void on_openCsv_clicked();
    void on_alterTable_clicked();
    void on_execute_clicked();
    void on_save_clicked();
    void on_copy_clicked();
    void on_paste_clicked();
    void on_delete__clicked();

    void on_clear_clicked();

private:

};

#endif // DATAIMPORTWIDGET2_H
