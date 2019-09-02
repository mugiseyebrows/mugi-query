#ifndef RICHHEADERVIEWTABFILTER_H
#define RICHHEADERVIEWTABFILTER_H

#include <QObject>
#include <QSet>

class RichHeaderTabFilter : public QObject
{
    Q_OBJECT
public:
    explicit RichHeaderTabFilter(QObject *parent = nullptr);

    void watch(QWidget* widget);

    QWidget* widget() const;

protected:

    bool eventFilter(QObject *watched, QEvent *event);

    QSet<QWidget*> mWatched;
    QWidget* mWidget;
signals:
    void tabPressed(bool shift);
public slots:
};

#endif // RICHHEADERVIEWTABFILTER_H
