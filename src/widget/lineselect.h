#ifndef LINESELECT_H
#define LINESELECT_H

#include <QWidget>

namespace Ui {
class LineSelect;
}

class LineSelect : public QWidget
{
    Q_OBJECT

public:
    enum Mode {
        ModeFileOpen,
        ModeFileSave,
        ModeDir
    };
    explicit LineSelect(QWidget *parent = nullptr);
    ~LineSelect();

    void setMode(Mode mode);

    void setCaption(const QString& caption);

    void setFilter(const QString& filter);

    QString text() const;

    void setText(const QString &text);
signals:
    void textChanged(QString);

protected slots:
    void on_select_clicked();

private:
    Ui::LineSelect *ui;
    Mode mMode = ModeFileOpen;
    QString mCaption;
    QString mFilter;
};

#endif // LINESELECT_H
