#ifndef ERROR_H
#define ERROR_H

class QWidget;
class QString;

class Error
{
public:
    static void show(QWidget* widget, const QString& error);
};

#endif // ERROR_H
