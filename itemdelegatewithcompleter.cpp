#include "itemdelegatewithcompleter.h"
#include <QLineEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QDebug>
#include <QAbstractItemView>

ItemDelegateWithCompleter::ItemDelegateWithCompleter(const QStringList& completions, QObject *parent)
    : QItemDelegate(parent), mCompletions(completions)
{

}

QWidget *ItemDelegateWithCompleter::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) const
{
    QLineEdit* edit = new QLineEdit(parent);
    QCompleter* completer = new QCompleter(mCompletions,edit);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    edit->setCompleter(completer);
    /*edit->installEventFilter(this);
    mLineEdit = edit;*/
    return edit;
}

void ItemDelegateWithCompleter::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor)) {
        lineEdit->setText(index.data().toString());
        return;
    }
    QItemDelegate::setEditorData(editor,index);
}

void ItemDelegateWithCompleter::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor)) {
        QCompleter* completer = lineEdit->completer();
        QString value = lineEdit->text();
        if (completer && completer->popup() && completer->popup()->isVisible()) {
            QModelIndex index = completer->popup()->currentIndex();
            if (index.isValid()) {
                value = completer->popup()->model()->data(index).toString();
            } else {
                value = completer->currentCompletion();
            }
        }
        model->setData(index,value);
        return;
    }
    QItemDelegate::setModelData(editor,model,index);
}
