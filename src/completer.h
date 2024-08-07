#ifndef COMPLETER_H
#define COMPLETER_H

#include <QCompleter>
#include "completerdata.h"

class Completer : public QCompleter
{
    Q_OBJECT
public:
    enum Context {
        Undefined,
        Unknown,
        Select,
        From,
        Join,
        Where,
        On,
        Update,
        Set,
    };
    Completer(QObject* parent = nullptr);
    void setContext(Context ctx);
    Context context() const;
    void setData(const CompleterData& data);
protected:
    Context mContext;
    CompleterData mData;
};


#endif // COMPLETER_H
