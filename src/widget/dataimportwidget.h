#ifndef DATAIMPORTWIDGET_H
#define DATAIMPORTWIDGET_H

#include <QWidget>

#include <QDialog>
#include <QModelIndex>
#include "tokens.h"

class RichHeaderView;
class CheckableStringListModel;

namespace Ui {
class DataImportWidget;
}

class DataImportWidget : public QWidget
{
    Q_OBJECT

public:
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
    void on_allColumns_clicked();
    void on_noneColumns_clicked();
    void onColumnsDataChanged(QModelIndex, QModelIndex, QVector<int>);
    void onDataPaste();

    void onTimeout();
    void updatePreview();
signals:
    void appendQuery(QString);

protected:


    CheckableStringListModel *columnsModel();
    RichHeaderView *headerView();
    void createHeaderViewWidgets();
    bool hasAnyData();
    void setColumnNames(const QStringList &names);

    Tokens mTokens;
    void newOrExistingTable();

    Ui::DataImportWidget *ui;
    QString mConnectionName;
    QList<QPair<QString, QString> > namesAndTypes();
};

#endif // DATAIMPORTWIDGET_H
