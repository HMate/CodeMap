#include "filesystem.h"

#include <QDir>

QString FileSystemManager::getCWD()
{
    return QDir::currentPath();
}
