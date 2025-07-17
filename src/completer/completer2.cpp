#include "completer2.h"

#include <QStringListModel>
#include "showhidefilter.h"
#include <QAbstractItemView>
#include "cursorcontext.h"
#include "schema2data.h"
#include "schema2tablesmodel.h"
#include "schema2relation.h"
#include "tolist.h"

Completer2::Completer2(QObject *parent): Completer(parent), mData(nullptr) {
    /*setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    setCaseSensitivity(Qt::CaseInsensitive);
    setCompletionMode(QCompleter::PopupCompletion);*/

#if 0
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
#endif
}

int Completer2::completionPrefixMinLength() const {
    return 3;
}

bool Completer2::needContext(const QString &completionPrefix) {
    QString prefix = this->completionPrefix();
    int l = completionPrefixMinLength();
    return prefix.mid(0, l) != completionPrefix.mid(0, l);
}

static QStringList sortedUnique(const QStringList& values) {
    QStringList res = toList(toSet(values));
    std::sort(res.begin(), res.end());
    return res;
}

static QStringList unsortedUnique(const QStringList& values) {
    return toList(toSet(values));
}

QStringList sorted(const QStringList& values) {
    QStringList res = values;
    std::sort(res.begin(), res.end());
    return res;
}

static QStringList sortedFields(const QStringList& fields) {

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

bool lessThanLength(const QString s1, const QString& s2) {
    return s1.size() < s2.size();
}

QStringListModel* stringModel(const QStringList& items, QObject* parent = nullptr) {
    QStringList res = items;
    std::sort(res.begin(), res.end(), lessThanLength);
    return new QStringListModel(res, parent);
}

void Completer2::setContext(const CursorContext& context)
{
    qDebug() << "Completer::setContext" << context;

    if (mContext == context) {
        qDebug() << "mContext == context";
        return;
    }
    popup()->hide();

    mContext = context;
    QStringListModel* model = nullptr;
    //QCompleter::ModelSorting sorting = CaseSensitivelySortedModel;
    if (context.isFieldContext()) {

        QStringList joinConditions;
#if 0
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
#endif

        QStringList items = sortedFields(joinConditions) + sortedFields(mCompleterData.fields) + sorted(mCompleterData.functions) + sorted(mCompleterData.keywords);
        model = stringModel(items, this);
        //sorting = UnsortedModel;
    } else if (context.isTableContext()) {
        model = stringModel(sortedUnique(mCompleterData.keywords + mCompleterData.tables), this);
    } else if (context.context == CursorContext::Call) {
        model = stringModel(sortedUnique(mCompleterData.procedures), this);
    } else if (context.context == CursorContext::Start) {
        model = stringModel(sortedUnique(mCompleterData.keywords), this);
    } else {
        qDebug() << "unexpected context" << context << __FILE__ << __LINE__;
        model = stringModel(sortedUnique(mCompleterData.keywords), this);
    }
    if (model) {
        //qDebug() << QString("set completer model %1 rows").arg(model->rowCount());
        setModel(model);
        //setModelSorting(sorting);
    }
}

CursorContext Completer2::context() const
{
    return mContext;
}

void Completer2::setData(const CompleterData& completerData, Schema2Data *schemaData)
{
    mCompleterData = completerData;
    mData = schemaData;
}


