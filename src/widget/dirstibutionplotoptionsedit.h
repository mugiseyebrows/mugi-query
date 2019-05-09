#ifndef DIRSTIBUTIONPLOTOPTIONSEDIT_H
#define DIRSTIBUTIONPLOTOPTIONSEDIT_H

#include <QWidget>

namespace Ui {
class DirstibutionPlotOptionsEdit;
}

class DirstibutionPlotOptionsEdit : public QWidget
{
    Q_OBJECT
public:
    enum Sender {
        SenderBins,
        SenderManualRange,
        SenderAutoRange,
        SenderRangeSelect
    };

    explicit DirstibutionPlotOptionsEdit(QWidget *parent = 0);
    ~DirstibutionPlotOptionsEdit();
    void init(int bins, double vmin, double vmax);
    void onValuesChanged(Sender sender);

    bool autoRange() const;

    int bins(bool *ok = 0) const;
    double vmin(bool *ok = 0) const;
    double vmax(bool *ok = 0) const;

signals:
    void valuesChanged(int,double,double);

private slots:
    void on_rangeAuto_clicked();

    void on_rangeManual_clicked();

private:
    Ui::DirstibutionPlotOptionsEdit *ui;
};

#endif // DIRSTIBUTIONPLOTOPTIONSEDIT_H
