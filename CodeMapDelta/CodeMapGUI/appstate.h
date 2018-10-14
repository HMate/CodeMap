#ifndef APPSTATE_H
#define APPSTATE_H

#include <vector>
#include <QString>
#include <QStringList>

#include "settings.h"

struct FileViewStateItem
{
    QString path;
    long long tabIndex;

    FileViewStateItem(const QString& path, long long tabIndex) : path(path), tabIndex(tabIndex) {}
};

/* Others can access the state of the application through this class.
 * State consists of things like
 * - last opened file, project
 * - app options
 * - last opened views
 *
 * Also handles the saving and loading of the states.
 * */
class AppStateHandler
{
    QString dataDirPath;
    QString lastOpenedDirPath;
    std::vector<FileViewStateItem> m_fileViews;

    Settings appSettings;
public:
    AppStateHandler();

    void addFileView(const QString& filePath, long long tabIndex);
    void removeFileView(const QString& filePath, long long tabIndex);
    const std::vector<FileViewStateItem>& getFileViews();

    Settings& settings();

    const QString& getLastOpenedDirectory();
    void setLastOpenedDirectory(const QString& path);

    //TODO: error handling for state save/load?
    void saveStateToDisk();
    void loadStateFromDisk();
};

#endif // APPSTATE_H
