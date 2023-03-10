#ifndef SCHEMA2PUSHVIEW_H
#define SCHEMA2PUSHVIEW_H

#include <QWidget>

namespace Ui {
class Schema2PushView;
}

class Schema2PushView : public QWidget
{
    Q_OBJECT

public:
    explicit Schema2PushView(QWidget *parent = nullptr);
    ~Schema2PushView();

private:
    Ui::Schema2PushView *ui;
};

#endif // SCHEMA2PUSHVIEW_H
