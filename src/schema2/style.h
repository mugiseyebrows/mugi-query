#ifndef PALETTE_H
#define PALETTE_H

#include <QColor>

class Style
{
public:
    Style();

    QColor SelectedBoundaryColor;
    QColor NormalBoundaryColor;
    QColor GradientColor0;
    QColor GradientColor1;
    QColor GradientColor2;
    QColor GradientColor3;
    QColor ArrowColor;
    QColor TitleColor;
    QColor BackgroundColor;
    QColor FineGridColor;
    QColor CoarseGridColor;
    QColor TextColor;
    QColor TextColorTitle;

    QColor ArrowColorFaded;
    QColor TextColorFaded;

    static Style dark() {
        Style res;
        res.SelectedBoundaryColor = QColor(255, 165, 0);
        res.NormalBoundaryColor = QColor(255, 255, 255);
        res.GradientColor0 = QColor("gray");
        res.GradientColor1 = QColor(80, 80, 80);
        res.GradientColor2 = QColor(64, 64, 64);
        res.GradientColor3 = QColor(58, 58, 58);
        res.ArrowColor = QColor("darkcyan");
        res.ArrowColorFaded = QColor("gray");
        res.TitleColor = QColor(139, 0, 139);
        res.BackgroundColor = QColor(53, 53, 53);
        res.FineGridColor = QColor(60,60,60);
        res.CoarseGridColor = QColor(25,25,25);
        res.TextColor = QColor("white");
        res.TextColorTitle = QColor("white");
        res.TextColorFaded = QColor("gray");
        return res;
    }

    static Style pumpkin() {
        Style res;
        res.SelectedBoundaryColor = QColor("#EB9F5E");
        res.NormalBoundaryColor = QColor("#F6ECDF");
        res.GradientColor0 = QColor("#665A58");
        res.GradientColor1 = QColor("#665A58");
        res.GradientColor2 = QColor("#4D4543");
        res.GradientColor3 = QColor("#4D4543");
        res.ArrowColor = QColor("#4D4543");
        res.ArrowColorFaded = QColor(Qt::lightGray);
        res.TitleColor = QColor("#F2A267");
        res.BackgroundColor = QColor("#F6ECDF");
        res.FineGridColor = QColor("#EDD4C4");
        res.CoarseGridColor = QColor("#EFC9AE");
        res.TextColor = QColor("#F2EAE3");
        res.TextColorTitle = QColor("#000000");
        res.TextColorFaded = QColor(Qt::lightGray);
        return res;
    }

    static Style bubblegum() {
        Style res;

        res.ArrowColor = QColor("#B7BAFF");
        res.ArrowColorFaded = QColor(Qt::lightGray);
        res.BackgroundColor = QColor("#ffffff");
        res.CoarseGridColor = QColor("#ffffff");
        res.FineGridColor = QColor("#ffffff");
        res.GradientColor0 = QColor("#F6E4EF");
        res.GradientColor1 = QColor("#F6E4EF");
        res.GradientColor2 = QColor("#EFEDFF");
        res.GradientColor3 = QColor("#EFEDFF");
        res.NormalBoundaryColor = QColor("#B7BAFF");
        res.SelectedBoundaryColor = QColor("#B7BAFF");
        res.TextColor = QColor("#000000");
        res.TextColorTitle = QColor(Qt::black);
        res.TextColorFaded = QColor(Qt::lightGray);
        res.TitleColor = QColor("#B7BAFF");
        return res;
    }

    static Style current;

};



#endif // PALETTE_H
