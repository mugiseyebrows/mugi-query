#ifndef DATAIMPORTWIDGET_H
#define DATAIMPORTWIDGET_H

#include <QWidget>

#include <QDialog>
#include <QModelIndex>
#include "tokens.h"
#include "field.h"

class DataImportModel;
class RichHeaderView;
class DataImportColumnModel;
class CallOnce;
class IntLineEdit;

class QLineEdit;
class QComboBox;
class QCheckBox;
class FieldAttributesWidget;
class ModelAppender;
class TableButtons;

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
        WidgetSize,
        WidgetFieldAttributes,
        WidgetForeignKey
    };

    enum ButtonIndex {
        ButtonInsertRow,
        ButtonRemoveRow,
        ButtonInsertColumn,
        ButtonRemoveColumn
    };

    explicit DataImportWidget(QWidget *parent = nullptr);
    ~DataImportWidget();

    void init(const QString& value);
    QString connectionName() const;
    void update(const Tokens &tokens);

public slots:
    void onUpdateTokens(QString connectionName, Tokens tokens);
protected slots:
    void onDataCopy();
    void onDataPaste();
    void onUpdatePreview();
    void onUpdateTitle();
    void onColumnTypeChanged(int);
    void onColumnNameChanged(int);
    void onFieldAttributeClicked(int);
    void on_newTable_textChanged(QString);
    void onModelSetTypes();
    void onSetColumnNamesAndTypes();
    void onColumnSizeChanged(int);
    void onColumnForeignKeyChanged(int);
signals:
    void appendQuery(QString);

protected:
    Ui::DataImportWidget *ui;
    QString mConnectionName;
    Tokens mTokens;
    CallOnce* mUpdatePreview;
    CallOnce* mSetModelTypes;
    CallOnce* mSetColumnNamesAndTypes;
    ModelAppender* mAppender;
    TableButtons* mButtons;
    QList<Field> mNewTableFields;

    RichHeaderView *headerView() const;
    void createHeaderViewWidgets();
    void setColumnNames(const QStringList &names);
    void setColumnTypes(const QList<QVariant::Type> &types);
    void newOrExistingTable();
    DataImportModel *dataModel() const;
    QString queries(bool preview);
    QWidget *widget(int row, int column) const;
    QLineEdit *widgetName(int column) const;
    QComboBox *widgetType(int column) const;
    IntLineEdit *widgetSize(int column) const;
    FieldAttributesWidget *widgetFieldAttributes(int column) const;
    QLineEdit *widgetForeignKey(int column) const;
    QString tableName();
    QList<Field> fields() const;
    bool newTable() const;
    static QVariant::Type guessType(QAbstractItemModel *model, const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void guessColumnType(int column);
    void setFields(const QList<Field> &fields);
    void setDataModelColumnCount(int count);

private slots:
    void on_clearData_clicked();
    void on_copyQuery_clicked();
    void on_abc_clicked();
    void on_format_currentIndexChanged(int index);
    void on_guessTypes_clicked();
    void on_existingTable_currentIndexChanged(int index);
    void on_optionNewTable_toggled(bool checked);
};

#endif // DATAIMPORTWIDGET_H
