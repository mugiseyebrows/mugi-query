#ifndef SCHEMA2VIEW_H
#define SCHEMA2VIEW_H

#include <QWidget>
#include <QModelIndex>

class QGraphicsScene;
class QPushButton;
class Schema2Data;
class Schema2TableModel;

namespace Ui {
class Schema2View;
}

class Schema2View : public QWidget
{
    Q_OBJECT

public:
    enum Mode {
        ModeNone,
        ModeMove,
        ModeRelate,
        ModeUnrelate,
        ModeAlter,
        ModeInsert,
        ModeSelect,
        ModeDrop
    };

    explicit Schema2View(QWidget *parent = nullptr);
    ~Schema2View();

    void setData(Schema2Data* data);

    Schema2Data* data() const;

    static QList<double> mScales;
    static QStringList mScalesText;

    Schema2TableModel* createTable(const QString &tableName);
public slots:
    void onTableClicked(QString tableName);

protected:
    void uncheckAllExcept(QPushButton *checked);

    Mode mMode;

    Schema2Data* mData;

    QStringList mTableStack;

    int mScaleIndex;

protected slots:
    void onFiterViewCurrentChanged(QModelIndex index, QModelIndex);
private slots:

    void on_move_clicked(bool checked);

    void on_relate_clicked(bool checked);

    void on_alter_clicked(bool checked);

    void on_insert_clicked(bool checked);

    void on_create_clicked();

    void on_arrange_clicked();

    void on_pull_clicked();

    void on_push_clicked();

    void on_zoomOut_clicked();

    void on_zoomIn_clicked();

    void on_scale_currentIndexChanged(int index);

    void on_select_clicked(bool checked);

    void on_relations_clicked();

    void on_filterLine_textChanged(const QString &arg1);

    void on_unrelate_clicked(bool checked);

    void on_drop_clicked(bool checked);

    friend class Automation;

    void on_script_clicked();

private:
    Ui::Schema2View *ui;
};

#endif // SCHEMA2VIEW_H
