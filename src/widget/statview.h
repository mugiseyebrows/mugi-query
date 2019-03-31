#ifndef STATVIEW_H
#define STATVIEW_H

#include <QTableView>

class StatView : public QTableView
{
    Q_OBJECT
public:
    StatView(QWidget *parent = nullptr);
};

#endif // STATVIEW_H
