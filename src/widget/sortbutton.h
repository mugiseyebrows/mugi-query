#ifndef SORTBUTTON_H
#define SORTBUTTON_H

#include <QPushButton>

class SortButton : public QPushButton
{
    Q_OBJECT
public:
    enum Order {
        None,
        Ascending,
        Descending
    };
    explicit SortButton(int column = 0, const QString& caption = {}, QWidget *parent = nullptr);

    void setOrder(Order order) {
        mOrder = order;
        updateIcon();
        emit(mColumn, mOrder);
    }

    void updateIcon() {
        switch (mOrder) {
            case None: setIcon(QIcon()); break;
            case Ascending: setIcon(QIcon()); break;
            case Descending: setIcon(QIcon()); break;
        }
    }

protected:

    int mColumn = 0;
    Order mOrder = None;

protected slots:

signals:
    void changed(int, int);

};

#endif // SORTBUTTON_H
