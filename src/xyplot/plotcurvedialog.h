#ifndef PLOTCURVEDIALOG_H
#define PLOTCURVEDIALOG_H

#include <QDialog>

class PlotCurve;

namespace Ui {
class PlotCurveDialog;
}

class PlotCurveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotCurveDialog(const QStringList& columns, PlotCurve* curve, QWidget *parent = nullptr);
    ~PlotCurveDialog();

    void updateCurve();

    bool remove() const;
protected:
    Ui::PlotCurveDialog *ui;
    PlotCurve* mCurve;
    bool mRemove = false;
private slots:
    void on_remove_clicked();
};

#endif // PLOTCURVEDIALOG_H
