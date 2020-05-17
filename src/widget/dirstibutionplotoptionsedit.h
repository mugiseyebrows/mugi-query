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
    void init(int bins, double min, double max);
    void onValuesChanged(Sender sender);

    bool autoRange() const;

    int bins(bool *ok = 0) const;
    double min(bool *ok = 0) const;
    double max(bool *ok = 0) const;

    void setManualRange(double vmin, double vmax);

protected:
    Ui::DirstibutionPlotOptionsEdit *ui;
    bool mSwitchMode;
    int mPrec;

signals:
    void valuesChanged(int,double,double);

protected slots:
    void on_autoRange_clicked();
    void on_manualRange_clicked();
};

#endif // DIRSTIBUTIONPLOTOPTIONSEDIT_H
