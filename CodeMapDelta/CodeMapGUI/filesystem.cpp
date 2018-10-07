#include "filesystem.h"

#include <QTextStream>
#include <QDebug>

QString FileSystemManager::getCWD()
{
    return QDir::currentPath();
}


QString FileSystemManager::concat(const QString& A, const QString& B)
{
    return QDir(A).filePath(B);
}

QDir FileSystemManager::getDirectory(const QString& path)
{
    return QFileInfo(path).absoluteDir();
}

bool FileSystemManager::saveFile(const QString& filePath, const QString& content)
{
    QDir dir = getDirectory(filePath);
    qWarning() << "checking path " << dir.absolutePath() << endl;
    if(!dir.exists())
    {
        qWarning() << "creating path " << dir.absolutePath() << endl;
        QDir().mkdir(dir.absolutePath());
    }

    QFile fileHandler(filePath);
    bool ok = fileHandler.open(QFile::WriteOnly);
    if(!ok)
    {
        auto errorMessage = QStringLiteral("Failed to open file: %1").arg(filePath);
        qWarning() << errorMessage << endl;
        return false;
    }
    QTextStream fileStream(&fileHandler);
    fileStream << content;
    return true;
}

bool FileSystemManager::doesFileExist(const QString& filePath)
{
	return QFile::exists(filePath);
}

QString FileSystemManager::readFile(const QString& filePath)
{
    QFile fileHandler(filePath);
    bool ok = fileHandler.open(QFile::ReadOnly | QFile::Text);
    if(!ok)
    {
        // create method: isFileOpenable? Exists?
        auto errorMessage = QStringLiteral("Failed to open file: %1").arg(filePath);
        qWarning() << errorMessage << endl;
        return "";
    }
    QTextStream fileStream(&fileHandler);
    QString result = fileStream.readAll();
    return result;

}
