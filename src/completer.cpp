#include "completer.h"

#include <QStringListModel>
#include "showhidefilter.h"
#include <QAbstractItemView>

Completer::Completer(QObject *parent) : mContext(Undefined), QCompleter(parent) {
    setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    setCaseSensitivity(Qt::CaseInsensitive);
    setCompletionMode(QCompleter::PopupCompletion);
    ShowHideFilter* filter = new ShowHideFilter(this);
    popup()->installEventFilter(filter);
    connect(filter, &ShowHideFilter::hidden, [=](){
        qDebug() << "popup hidden, reset completer context";
        setContext(Undefined);
    });
    connect(filter, &ShowHideFilter::shown, [=](){
        qDebug() << "popup shown, set first item";
        popup()->setCurrentIndex(popup()->model()->index(0, 0));
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

void Completer::setContext(Context context)
{
    static QSet<Context> fieldContexts = {Select, On, Where, Set};
    static QSet<Context> tableContexts = {From, Join, Update, Table, To};
    mContext = context;
    QStringListModel* model = nullptr;
    QCompleter::ModelSorting sorting = CaseSensitivelySortedModel;
    if (fieldContexts.contains(context)) {
        QStringList items = sortedFields(mData.fields) + sorted(mData.functions) + sorted(mData.keywords);
        model = new QStringListModel(items, this);
        sorting = UnsortedModel;
    } else if (tableContexts.contains(context)) {
        model = new QStringListModel(sortedUnique(mData.keywords + mData.tables), this);
    } else {
        qDebug() << "not implemented for context" << context;
    }
    if (model) {
        qDebug() << QString("set completer model %1 rows").arg(model->rowCount());
        setModel(model);
        setModelSorting(sorting);
    }
}

Completer::Context Completer::context() const
{
    return mContext;
}

void Completer::setData(const CompleterData &data)
{
    mData = data;
}


