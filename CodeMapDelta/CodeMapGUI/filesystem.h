#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QString>
#include <QDir>

/*
 * Manages where are we in the FileSystem,
 * and stores/handles paths that are important for the application
 * like: save path, project paths, current working dir, files in dir
 * */
class FileSystemManager
{
public:
    static QString getCWD();
    static QDir getDirectory(const QString& path);
    static QString concat(const QString& A, const QString& B);

	static bool doesFileExist(const QString& filePath);
    static bool saveFile(const QString& path, const QString& content);
    static QString readFile(const QString& filePath);

    static QString getDataFolder();
    static QString getGuiIconsFolder();
};

using FS = FileSystemManager;

#endif // FILESYSTEM_H
