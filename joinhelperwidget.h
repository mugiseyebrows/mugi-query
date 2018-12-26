#ifndef JOINHELPERWIDGET_H
#define JOINHELPERWIDGET_H

#include <QWidget>
#include "tokens.h"

#include <QModelIndex>

class ModelAppender;

namespace Ui {
class JoinHelperWidget;
}

class JoinHelperWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JoinHelperWidget(QWidget *parent = nullptr);
    ~JoinHelperWidget() override;

    void init(const QString &connectionName);
    void update(const Tokens &tokens);

    void closeEvent(QCloseEvent *event) override;

public slots:
    void onTablesDataChanged(QModelIndex, QModelIndex);
protected:
    QString relationsPath() const;
    void findPath();
    void saveRelationsModel();
    void loadRelationsModel();
signals:
    void appendQuery(QString,bool);

protected slots:
    void onTablesRowInserted(int);
    void onSelectLastTablesRow();
private slots:

    void onAdjustSplitters();

    void on_copy_clicked();

    void on_joinType_currentIndexChanged(int index);

    void on_copyToNewSession_clicked();

private:
    Ui::JoinHelperWidget *ui;
    QString mConnectionName;
    Tokens mTokens;
    ModelAppender* mRelationsAppender;
    ModelAppender* mTablesAppender;
};

#endif // JOINHELPERWIDGET_H