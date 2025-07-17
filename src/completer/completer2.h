#ifndef COMPLETER2_H
#define COMPLETER2_H

#include <QCompleter>
#include "completerdata.h"
#include "cursorcontext.h"
#include "completer.h"

class Schema2Data;

class Completer2 : public Completer
{
    Q_OBJECT
public:

    Completer2(QObject* parent = nullptr);

    bool needContext(const QString& completionPrefix);

    void setContext(const CursorContext& context);
    CursorContext context() const;
    void setData(const CompleterData& completerData, Schema2Data *schemaData);
    int completionPrefixMinLength() const;
protected:
    CursorContext mContext;
    CompleterData mCompleterData;
    Schema2Data* mData;

};



#endif // COMPLETER2_H
