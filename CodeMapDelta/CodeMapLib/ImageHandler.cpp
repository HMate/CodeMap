#include "ImageHandler.h"

#include "FileSystem.h"

QImage ImageHandler::loadIcon(const QString& name)
{
    return QImage(QString("%1/%2").arg(FS::getGuiIconsFolder()).arg(name));
}
