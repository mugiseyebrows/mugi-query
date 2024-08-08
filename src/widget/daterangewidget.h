#ifndef DATERANGEWIDGET_H
#define DATERANGEWIDGET_H

#include <QWidget>

namespace Ui {
class DateRangeWidget;
}

class DateRangeWidget : public QWidget
{
    Q_OBJECT
public:
    enum Mode {
        NDays,
        All,
        Range
    };

    explicit DateRangeWidget(QWidget *parent = nullptr);
    ~DateRangeWidget();

    Mode mode() const;

    int days() const;

    QDate date1() const;
    QDate date2() const;

signals:
    void expanded();
    void contracted();
    void changed(int, int, QDate, QDate);

private slots:
    void on_combo_currentIndexChanged(int index);
    void on_date1_dateChanged(const QDate &date);
    void on_date2_dateChanged(const QDate &date);
private:
    Ui::DateRangeWidget *ui;
};


#endif // DATERANGEWIDGET_H
