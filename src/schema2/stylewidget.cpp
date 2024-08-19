#include "stylewidget.h"
#include "ui_stylewidget.h"

#include "style.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QRegularExpression>
#include "schema2view.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

Schema2View* schemaView() {
    auto widgets = qApp->topLevelWidgets();
    for(auto* widget: widgets) {
        if (Schema2View* view = qobject_cast<Schema2View*>(widget)) {
            return view;
        }
    }
    return nullptr;
}

StyleWidget::StyleWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StyleWidget)
{
    ui->setupUi(this);

    QStringList names = {
        "SelectedBoundaryColor",
        "NormalBoundaryColor",
        "GradientColor0",
        "GradientColor1",
        "GradientColor2",
        "GradientColor3",
        "ArrowColor",
        "TitleColor",
        "BackgroundColor",
        "FineGridColor",
        "CoarseGridColor",
        "TextColor"
    };

    QGridLayout* layout = new QGridLayout();
    for(int row=0;row<names.size();row++) {
        QLabel* label = new QLabel(names[row]);
        QLineEdit* edit = new QLineEdit();
        layout->addWidget(label, row, 0);
        layout->addWidget(edit, row, 1);
        mEdits.append(edit);
        connect(edit, &QLineEdit::textChanged, [=](const QString& text){
            qDebug() << "textChanged row" << row;
            QRegularExpression rx("^#?([0-9a-z]{6})$", QRegularExpression::CaseInsensitiveOption);
            auto m = rx.match(text);
            if (!m.hasMatch()) {
                qDebug() << "!m.hasMatch()";
                return;
            }
            qDebug() << "color" << "#" + m.captured(1);
            QColor color("#" + m.captured(1));
            switch(row) {
            case 0: Style::current.SelectedBoundaryColor = color; break;
            case 1: Style::current.NormalBoundaryColor = color; break;
            case 2: Style::current.GradientColor0 = color; break;
            case 3: Style::current.GradientColor1 = color; break;
            case 4: Style::current.GradientColor2 = color; break;
            case 5: Style::current.GradientColor3 = color; break;
            case 6: Style::current.ArrowColor = color; break;
            case 7: Style::current.TitleColor = color; break;
            case 8: Style::current.BackgroundColor = color; break;
            case 9: Style::current.FineGridColor = color; break;
            case 10: Style::current.CoarseGridColor = color; break;
            case 11: Style::current.TextColor = color; break;
            }
            schemaView()->updateView();
        });
    }

    //setLayout(layout);
    ui->widget->setLayout(layout);
}

StyleWidget::~StyleWidget()
{
    delete ui;
}



void StyleWidget::on_save_clicked()
{
    QStringList names = {
        "SelectedBoundaryColor",
        "NormalBoundaryColor",
        "GradientColor0",
        "GradientColor1",
        "GradientColor2",
        "GradientColor3",
        "ArrowColor",
        "TitleColor",
        "BackgroundColor",
        "FineGridColor",
        "CoarseGridColor",
        "TextColor"
    };

    QString path = QFileDialog::getSaveFileName(this, "", "", "Json files (*.json)");
    if (path.isEmpty()) {
        return;
    }

    QVariantHash values;

    for(int row=0;row<names.size();row++) {
        values[names[row]] = "#" + mEdits[row]->text();
    }

    QJsonDocument doc = QJsonDocument::fromVariant(values);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", QString("Cannot open file %1").arg(path));
        return;
    }
    file.write(doc.toJson());

}

