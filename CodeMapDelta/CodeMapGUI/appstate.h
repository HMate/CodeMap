#ifndef APPSTATE_H
#define APPSTATE_H

#include <vector>
#include <QString>
#include <QStringList>

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
public:
    AppStateHandler();

    void addFileView(const QString& filePath);
    void removeFileView(const QString& filePath);
    const QStringList& getFileViews();

    const QString& getLastOpenedDirectory();
    void setLastOpenedDirectory(const QString& path);

    //TODO: error handling for state save/load?
    void saveStateToDisk();
    void loadStateFromDisk();
};

#endif APPSTATE_H
