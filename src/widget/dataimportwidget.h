#ifndef DATAIMPORTWIDGET_H
#define DATAIMPORTWIDGET_H

#include <QWidget>

#include <QDialog>
#include <QModelIndex>
#include "tokens.h"

class DataImportModel;
class RichHeaderView;
class DataImportColumnModel;
class CallOnce;

namespace Ui {
class DataImportWidget;
}

class DataImportWidget : public QWidget
{
    Q_OBJECT

public:
    enum WidgetIndex {
        WidgetName,
        WidgetType
    };

    explicit DataImportWidget(QWidget *parent = nullptr);
    ~DataImportWidget();

    void init(const QString& value);

    QString connectionName() const;

public slots:
    void update(const Tokens &tokens);

protected slots:
    void onExistingTableCurrentIndexChanged(int index);
    void on_optionNewTable_clicked();
    void on_optionExistingTable_clicked();

    void onColumnDataChanged(int, QModelIndex, QModelIndex);
    void onDataCopy();
    void onDataPaste();

    void onUpdatePreview();
    void onColumnTypeChanged(int column);
    void onColumnNameChanged(int column);
    void on_newTable_textChanged(QString);
    //void onDataChanged(QModelIndex, QModelIndex, QVector<int>);
    void onSetTypes();
signals:
    void appendQuery(QString);

protected:

    RichHeaderView *headerView();
    void createHeaderViewWidgets();

    void setColumnNames(const QStringList &names);
    void setColumnTypes(const QList<QVariant::Type> &types);

    void newOrExistingTable();

    Ui::DataImportWidget *ui;
    QString mConnectionName;
    Tokens mTokens;
    void namesAndTypes(QStringList &names, QStringList &types);
    DataImportModel *dataModel();

    QString queries(bool preview);

    CallOnce* mUpdatePreview;
    CallOnce* mSetTypes;

private slots:
    void on_clearData_clicked();
    void on_copyQuery_clicked();
    void on_abc_clicked();
};

#endif // DATAIMPORTWIDGET_H
