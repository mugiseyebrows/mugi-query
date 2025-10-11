#include "contrasting.h"


double luminance(const QColor& color) {
    const double RED = 0.2126;
    const double GREEN = 0.7152;
    const double BLUE = 0.0722;
    auto comp = [](float c) {
        const double GAMMA = 2.4;
        if (c <= 0.03928) {
            return c / 12.92;
        }
        return pow((c + 0.055) / 1.055, GAMMA);
    };
    float r, g, b;
    color.getRgbF(&r, &g, &b);
    return comp(r) * RED + comp(g) * GREEN + comp(b) * BLUE;
}

double contrast(const QColor& c1, const QColor& c2) {
    double lum1 = luminance(c1);
    double lum2 = luminance(c2);
    double brightest = std::max(lum1, lum2);
    double darkest = std::min(lum1, lum2);
    return (brightest + 0.05) / (darkest + 0.05);
}

QColor contrasting(const QColor& background, const QColor& text1, const QColor& text2) {
    if (contrast(text1, background) > contrast(text2, background)) {
        return text1;
    }
    return text2;
}
