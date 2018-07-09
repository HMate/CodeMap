#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QString>

/*
 * Manages where are we in the filesystem,
 * and stores/handles paths that are important for the application
 * like: save path, project paths, current working dir, files in dir
 * */
class FileSystemManager
{
public:
    static QString getCWD();
    static QString concat(const QString& A, const QString& B);

    static bool saveFile(const QString& path, const QString content);
    static QString readFile(const QString& filePath);
};

using FS = FileSystemManager;

#endif // FILESYSTEM_H
