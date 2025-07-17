#include "sortbutton.h"

SortButton::SortButton(int column, const QString& caption, QWidget *parent)
    : QPushButton{caption, parent}, mColumn(column)
{
    connect(this, &QPushButton::clicked, [=](){

        switch (mOrder) {
            case None: setOrder(Ascending); break;
            case Ascending: setOrder(Descending); break;
            case Descending: setOrder(Ascending); break;
        }

    });
}
