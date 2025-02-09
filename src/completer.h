#ifndef COMPLETER_H
#define COMPLETER_H

#include <QCompleter>
#include "completerdata.h"
#include "cursorcontext.h"

class Schema2Data;

class Completer : public QCompleter
{
    Q_OBJECT
public:

    Completer(QObject* parent = nullptr);
    void setContext(const CursorContext& context);
    CursorContext context() const;
    void setData(const CompleterData& completerData, Schema2Data *schemaData);
protected:
    CursorContext mContext;
    CompleterData mCompleterData;
    Schema2Data* mData;

};



#endif // COMPLETER_H
