#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QDialog>

namespace Ui {
class ColorDialog;
}

class ColorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorDialog(QWidget *parent = nullptr);
    ~ColorDialog();

    void setColor(const QColor &color, bool setRgb, bool setHsv, bool setHex);

    QColor color() const;

protected:
    bool mListen = true;

public slots:
    void onColorClicked(QColor color);
protected slots:
    void onRgb(int value);
    void onHsv(int value);
    void onHex(const QString &hex);
private:
    Ui::ColorDialog *ui;
};

#endif // COLORDIALOG_H
