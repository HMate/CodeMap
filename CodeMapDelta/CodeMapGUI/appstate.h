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
    QStringList fileViews;
public:
    AppStateHandler();

    void addFileView(const QString& filePath);
    const QStringList& getFileViews();

    //TODO: error handling for state save/load?
    void saveStateToDisk();
    void loadStateFromDisk();
};
