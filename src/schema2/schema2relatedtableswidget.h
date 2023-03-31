#ifndef SCHEMA2RELATEDTABLESWIDGET_H
#define SCHEMA2RELATEDTABLESWIDGET_H

#include <QWidget>

namespace Ui {
class Schema2RelatedTablesWidget;
}

class Schema2TablesModel;
class QStringListModel;

class Schema2RelatedTablesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Schema2RelatedTablesWidget(Schema2TablesModel* tables, const QString& table, QWidget *parent = nullptr);
    ~Schema2RelatedTablesWidget();

protected:
    Schema2TablesModel* mTables;
    QStringListModel* mOutput;

    void listRelated(QStringListModel *input, QStringListModel *output);
private:
    Ui::Schema2RelatedTablesWidget *ui;
};

#endif // SCHEMA2RELATEDTABLESWIDGET_H
