#include "showandraise.h"

#include <QWidget>

void showAndRaise(QWidget* widget) {
    //widget->showNormal();
    widget->show();
    widget->setWindowState(widget->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    //widget->activateWindow();
    widget->raise();
}
