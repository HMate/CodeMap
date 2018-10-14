#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include <QImage>

class ImageHandler
{
public:
    static QImage loadIcon(const QString& name);
};

namespace icons
{
const QString Plus = "plus.png";
const QString Minus = "minus.png";
}

#endif // IMAGEHANDLER_H
