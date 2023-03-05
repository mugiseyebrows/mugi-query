#ifndef SCHEMA2ALTERVIEW_H
#define SCHEMA2ALTERVIEW_H

#include <QWidget>

namespace Ui {
class Schema2AlterView;
}

class Schema2TableModel;

class Schema2AlterView : public QWidget
{
    Q_OBJECT

public:
    explicit Schema2AlterView(QWidget *parent = nullptr);
    ~Schema2AlterView();

    void setModel(Schema2TableModel* model);

protected:
    Schema2TableModel* mModel;

private:
    Ui::Schema2AlterView *ui;
};

#endif // SCHEMA2ALTERVIEW_H
