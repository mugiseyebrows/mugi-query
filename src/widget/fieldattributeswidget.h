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

    bool index() const;

    bool unique() const;

    void setPrimaryKey(bool value);

    void setAutoincrement(bool value);

    void setIndex(bool value);

    void setUnique(bool value);

signals:
    void primaryKeyClicked(bool);
    void autoincrementClicked(bool);
    void indexClicked(bool);
    void uniqueClicked(bool);
    void attributeClicked(bool);

private:
    Ui::FieldAttributesWidget *ui;
};

#endif // FIELDATTRIBUTESWIDGET_H
