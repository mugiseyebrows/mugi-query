#ifndef SCHEMA2ZOOMTOOLBAR_H
#define SCHEMA2ZOOMTOOLBAR_H

#include <QWidget>

namespace Ui {
class Schema2ZoomToolbar;
}

class Schema2ZoomToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit Schema2ZoomToolbar(QWidget *parent = nullptr);
    ~Schema2ZoomToolbar();

    static QList<double> mScales;
    static QStringList mScalesText;


signals:
    void zoom(double);

protected slots:
    void on_zoomOut_clicked();
    void on_zoomIn_clicked();
    void on_scale_currentIndexChanged(int index);
protected:
    Ui::Schema2ZoomToolbar *ui;
    int mScaleIndex;
};

#endif // SCHEMA2ZOOMTOOLBAR_H
