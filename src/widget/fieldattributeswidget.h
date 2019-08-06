#ifndef FIELDATTRIBUTESWIDGET_H
#define FIELDATTRIBUTESWIDGET_H

#include <QWidget>

namespace Ui {
class FieldAttributesWidget;
}

class FieldAttributesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FieldAttributesWidget(QWidget *parent = nullptr);
    ~FieldAttributesWidget();

    bool primaryKey() const;

    bool autoincrement() const;

signals:
    void primaryKeyClicked(bool);
    void autoincrementClicked(bool);

private slots:
    void on_primaryKey_stateChanged(int state);

private:
    Ui::FieldAttributesWidget *ui;
};

#endif // FIELDATTRIBUTESWIDGET_H
