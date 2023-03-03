#ifndef SCHEMA2VIEW_H
#define SCHEMA2VIEW_H

#include <QWidget>

class QGraphicsScene;

namespace Ui {
class Schema2View;
}

class Schema2View : public QWidget
{
    Q_OBJECT

public:
    explicit Schema2View(QWidget *parent = nullptr);
    ~Schema2View();

    void setScene(QGraphicsScene* scene);

private slots:

    void on_move_clicked(bool checked);

    void on_relate_clicked(bool checked);

    void on_alter_clicked(bool checked);

    void on_insert_clicked(bool checked);

    void on_create_clicked();

private:
    Ui::Schema2View *ui;
};

#endif // SCHEMA2VIEW_H
