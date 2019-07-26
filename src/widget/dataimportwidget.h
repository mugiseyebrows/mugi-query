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

class QLineEdit;
class QComboBox;
class QCheckBox;

namespace Ui {
class DataImportWidget;
}

class DataImportWidget : public QWidget
{
    Q_OBJECT

public:
    enum WidgetIndex {
        WidgetName,
        WidgetType,
        WidgetPrimaryKey,
    };

    explicit DataImportWidget(QWidget *parent = nullptr);
    ~DataImportWidget();

    void init(const QString& value);

    QString connectionName() const;

    static QVariant::Type guessType(QAbstractItemModel *model, const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void guessColumnType(int column);
    void update(const Tokens &tokens);

public slots:
    void onUpdateTokens(QString connectionName, Tokens tokens);
protected slots:
    //void onExistingTableCurrentIndexChanged(int index);
    void on_optionNewTable_clicked();
    void on_optionExistingTable_clicked();

    //void onColumnDataChanged(int, QModelIndex, QModelIndex);
    void onDataCopy();
    void onDataPaste();

    void onUpdatePreview();
    void onUpdateTitle();
    void onColumnTypeChanged(int);
    void onColumnNameChanged(int);
    void onColumnPrimaryKeyClicked(int);
    void on_newTable_textChanged(QString);
    //void onDataChanged(QModelIndex, QModelIndex, QVector<int>);
    void onModelSetTypes();
    void onSetColumnNamesAndTypes();

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
    void namesTypesAndKeys(QStringList &names, QStringList &types, QList<bool> &primaryKeys);
    DataImportModel *dataModel();

    QString queries(bool preview);

    CallOnce* mUpdatePreview;
    CallOnce* mSetModelTypes;
    CallOnce* mSetColumnNamesAndTypes;

    QWidget *widget(int row, int column);
    QLineEdit *widgetName(int column);
    QComboBox *widgetType(int column);
    QCheckBox *widgetPrimaryKey(int column);

    QString tableName();
private slots:
    void on_clearData_clicked();
    void on_copyQuery_clicked();
    void on_abc_clicked();
    void on_format_currentIndexChanged(int index);
    void on_guessTypes_clicked();
    void on_existingTable_currentIndexChanged(int index);
};

#endif // DATAIMPORTWIDGET_H
