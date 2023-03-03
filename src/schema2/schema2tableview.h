#ifndef SCHEMA2TABLEVIEW_H
#define SCHEMA2TABLEVIEW_H

#include <QWidget>

class Schema2TableModel;

namespace Ui {
class Schema2TableView;
}

class Schema2TableView : public QWidget
{
    Q_OBJECT

public:
    explicit Schema2TableView(QWidget *parent = nullptr);
    ~Schema2TableView();

    void setModel(Schema2TableModel* model);

private:
    Ui::Schema2TableView *ui;
};

#endif // SCHEMA2TABLEVIEW_H
