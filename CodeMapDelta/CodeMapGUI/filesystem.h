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
};

#endif // FILESYSTEM_H
