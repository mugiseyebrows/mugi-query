#ifndef ACTION_H
#define ACTION_H

#include <QVariantList>
#include <functional>

class Function {
public:
    Function() {

    }
    Function(const QString& name, const std::function<void()>& fn) : name(name), fn(fn) {

    }
    QString name;
    std::function<void()> fn;
};

class Action {
public:
    enum Type {
        ActionEmpty,
        ActionConnectToDatabaseFromHistory,
        ActionAppendQuery,
        ActionExecuteCurrentQuery,
        ActionShowSaveDataDialog,
        ActionSetXYPlot,
        ActionSetDistributionPlot,
        ActionSetDistributionPlotManualRange,
        ActionShowJoinHelper,
        ActionShowDistributionPlot,
        ActionShowXYPlot,
        ActionShowDataImportDialog,
        ActionShowDateTimeRangeWidget,
        ActionShowQueryHistory,
        ActionShowDatabaseHistory,
        ActionSaveStat,
        ActionSaveQuery,
        ActionCompareData,
        ActionToolMysql,
        ActionToolMysqldump,
        ActionSchemaEdit,
        ActionToolXJoin,
        ActionCreateTable,
        ActionCreateRelation,
        ActionCreatePrimaryKey,
        ActionPushSchema,
        ActionCompareTable,
        ActionShowAlterView,
        ActionSelectTablesNone,
        ActionSelectTablesAll,
        ActionSelectTables,
        ActionExportTo,
        ActionRunSteps,
        ActionPullSchema,
    };
    Action(Type type = ActionEmpty, const QVariantList& args = QVariantList());
    Action(const QList<Function>& functions) : mType(ActionRunSteps), mFunctions(functions) {

    }
    Type type() const;
    QVariantList args() const;
    QVariant arg(int index) const;
    QList<Function> functions() const;

    Type mType;
    QVariantList mArgs;
    QList<Function> mFunctions;
};


#endif // ACTION_H
