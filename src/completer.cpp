#include "completer.h"

#include <QStringListModel>
#include "showhidefilter.h"
#include <QAbstractItemView>
#include "cursorcontext.h"
#include "schema2data.h"
#include "schema2tablesmodel.h"
#include "schema2relation.h"

Completer::Completer(QObject *parent): QCompleter(parent), mData(nullptr) {
    setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    setCaseSensitivity(Qt::CaseInsensitive);
    setCompletionMode(QCompleter::PopupCompletion);
    ShowHideFilter* filter = new ShowHideFilter(this);
    popup()->installEventFilter(filter);
    connect(filter, &ShowHideFilter::hidden, [=](){
        qDebug() << "popup hidden, reset completer context";
        setContext({});
    });
    connect(filter, &ShowHideFilter::shown, [=](){
        //qDebug() << "popup shown, set first item";
        auto* model = popup()->model();
        popup()->setCurrentIndex(model->index(0, 0));
    });
}

template <typename T>
static QSet<T> toSet(const QList<T>& qlist)
{
    return QSet<T> (qlist.constBegin(), qlist.constEnd());
}

template <typename T>
static QList<T> toList(const QSet<T>& qlist)
{
    return QList<T> (qlist.constBegin(), qlist.constEnd());
}

QStringList sortedUnique(const QStringList& values) {
    QStringList res = toList(toSet(values));
    std::sort(res.begin(), res.end());
    return res;
}

QStringList sorted(const QStringList& values) {
    QStringList res = values;
    std::sort(res.begin(), res.end());
    return res;
}

static QStringList sortedFields(const QStringList fields) {

    QStringList shortName;
    QStringList fullName;
    for(const QString& name: fields) {
        if (name.contains('.')) {
            fullName.append(name);
        } else {
            shortName.append(name);
        }
    }
    return sortedUnique(shortName) + sortedUnique(fullName);
}

void Completer::setContext(const CursorContext& context)
{
    qDebug() << "Completer::setContext" << context;

    if (mContext == context) {
        qDebug() << "mContext == context";
        return;
    }
    popup()->hide();

    mContext = context;
    QStringListModel* model = nullptr;
    QCompleter::ModelSorting sorting = CaseSensitivelySortedModel;
    if (context.isFieldContext()) {
        QStringList joinConditions;
        if (context.context == CursorContext::On) {
            if (!context.table.isEmpty()) {
                SNames tables = mData->tables()->tableNames();
                int index = indexOf(tables, context.table);
                if (index > -1) {
                    SName name = tables.names[index];
                    QList<Schema2Relation *> relations = mData->tables()->relationsFrom(name) + mData->tables()->relationsTo(name);
                    for(Schema2Relation * relation: relations) {
                        joinConditions.append(relation->asRelation().joinCondition(true));
                        joinConditions.append(relation->asRelation().joinCondition(false));
                    }
                }
            }
        }

        QStringList items = sortedFields(joinConditions) + sortedFields(mCompleterData.fields) + sorted(mCompleterData.functions) + sorted(mCompleterData.keywords);
        model = new QStringListModel(items, this);
        sorting = UnsortedModel;
    } else if (context.isTableContext()) {
        model = new QStringListModel(sortedUnique(mCompleterData.keywords + mCompleterData.tables), this);
    } else if (context.context == CursorContext::Call) {
        model = new QStringListModel(sortedUnique(mCompleterData.procedures), this);
    } else if (context.context == CursorContext::Start) {
        model = new QStringListModel(sortedUnique(mCompleterData.keywords), this);
    } else {
        qDebug() << "unexpected context" << context << __FILE__ << __LINE__;
        model = new QStringListModel(sortedUnique(mCompleterData.keywords), this);
    }
    if (model) {
        //qDebug() << QString("set completer model %1 rows").arg(model->rowCount());
        setModel(model);
        setModelSorting(sorting);
    }
}

CursorContext Completer::context() const
{
    return mContext;
}

void Completer::setData(const CompleterData& completerData, Schema2Data *schemaData)
{
    mCompleterData = completerData;
    mData = schemaData;
}


