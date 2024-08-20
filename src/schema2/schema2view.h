#ifndef SCHEMA2VIEW_H
#define SCHEMA2VIEW_H

#include <QWidget>
#include <QModelIndex>

class QGraphicsScene;
class QPushButton;
class Schema2Data;
class Schema2TableModel;
class Schema2TablesModel;
class Schema2TableItem;

namespace Ui {
class Schema2View;
}

class ExportParams {

};

class Schema2View : public QWidget
{
    Q_OBJECT

public:

    explicit Schema2View(QWidget *parent = nullptr);
    ~Schema2View();

    void setData(Schema2Data* data);

    Schema2Data* data() const;

    Schema2TableModel* createTable(const QString &tableName);

    QRectF sceneRect() const;

    QGraphicsScene* scene() const;

    Schema2TablesModel* tables() const;

    void updateView();

public slots:
    void onTableClicked(QString tableName, QPointF scenePos);

protected:
    void uncheckAllExcept(QPushButton *checked);

    Schema2Data* mData;

    //QStringList mTableStack;

    QList<Schema2TableItem *> tablesAt(const QPointF &pos) const;
    void setTablesMovable(bool on);
protected slots:
    void onFiterViewCurrentChanged(QModelIndex index, QModelIndex);
    void onCreate();
    void onArrange();
    void onScript();
    void onSave();

    void onPull();
    void onPush();
private slots:

    void on_filterLine_textChanged(const QString &arg1);

    friend class Automation;

    void on_checkAll_clicked();

    void on_uncheckAll_clicked();

    void on_grayed_clicked();

    void on_invisible_clicked();

    void on_editStyle_clicked();

    void on_style_currentTextChanged(const QString &arg1);

private:
    Ui::Schema2View *ui;
};

#endif // SCHEMA2VIEW_H
