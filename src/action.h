#ifndef ACTION_H
#define ACTION_H

#include <QVariantList>

class Action {
public:
    enum Type {
        ActionEmpty,
        ActionAddDatabaseFromHistory,
        ActionAppendQuery,
        ActionExecuteCurrentQuery,
        ActionShowSaveDataDialog,
        ActionSetXYPlot,
    };
    Action();
    Action(Type type, const QVariantList& args = QVariantList());
    Type type() const;
    QVariantList args() const;
    QVariant arg(int index) const;

    Type mType;
    QVariantList mArgs;
};


#endif // ACTION_H
