#ifndef APPSTATE_H
#define APPSTATE_H

#include <vector>
#include <QString>
#include <QStringList>

#include "settings.h"

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
    QStringList fileViews;

	Settings appSettings;
public:
    AppStateHandler();

    void addFileView(const QString& filePath);
    void addFileView(const QString& filePath, long long tabViewIndex);
    void removeFileView(const QString& filePath);
    void removeFileView(const QString& filePath, long long tabViewIndex);
    const QStringList& getFileViews();

	Settings& settings();

    const QString& getLastOpenedDirectory();
    void setLastOpenedDirectory(const QString& path);

    //TODO: error handling for state save/load?
    void saveStateToDisk();
    void loadStateFromDisk();
};

#endif APPSTATE_H
