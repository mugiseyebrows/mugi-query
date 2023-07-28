#ifndef ACTION_H
#define ACTION_H

#include <QVariantList>

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
    };
    Action(Type type = ActionEmpty, const QVariantList& args = QVariantList());
    Type type() const;
    QVariantList args() const;
    QVariant arg(int index) const;

    Type mType;
    QVariantList mArgs;
};


#endif // ACTION_H
