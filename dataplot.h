#ifndef DATAPLOT_H
#define DATAPLOT_H

#include <QWidget>

#include "tokens.h"
#include <QModelIndex>

class ModelAppender;
class QSqlQueryModel;

namespace Ui {
class DataPlot;
}

class DataPlot : public QWidget
{
    Q_OBJECT

public:
    enum cols {
        col_x,
        col_y,
        col_line,
        col_marker,
        cols_size
    };

    explicit DataPlot(QWidget *parent = nullptr);
    ~DataPlot();

    void setModel(QSqlQueryModel* model);

protected:
    Ui::DataPlot *ui;
    QSqlQueryModel* mModel;
    QList<QStringList> mXy;
    ModelAppender* mAppender;

protected slots:
    void onModelDataChanged(QModelIndex, QModelIndex, QVector<int>);
    void setDefaultColors();
};

#endif // DATAPLOT_H
