#ifndef COPYFORMATWIDGET_H
#define COPYFORMATWIDGET_H

#include <QWidget>

#include "copyformat2.h"

namespace Ui {
class CopyFormatWidget;
}

class CopyFormatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CopyFormatWidget(QWidget *parent = nullptr);
    ~CopyFormatWidget();

    CopyFormat2 format() const;

    void setKey(const QStringList& key);

signals:
    void keyClicked(QStringList);

private slots:
    void on_format_currentIndexChanged(int index);

    void on_key_clicked();

private:
    Ui::CopyFormatWidget *ui;
    QStringList mKey;
};

#endif // COPYFORMATWIDGET_H
