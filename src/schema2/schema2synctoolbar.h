#ifndef SCHEMA2SYNCTOOLBAR_H
#define SCHEMA2SYNCTOOLBAR_H

#include <QWidget>

namespace Ui {
class Schema2SyncToolbar;
}

class Schema2SyncToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit Schema2SyncToolbar(QWidget *parent = nullptr);
    ~Schema2SyncToolbar();

signals:
    void pull();
    void push();

private:
    Ui::Schema2SyncToolbar *ui;
};

#endif // SCHEMA2SYNCTOOLBAR_H
