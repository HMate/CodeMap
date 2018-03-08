
#include <QString>
#include <QTextEdit>
#include <QMdiSubWindow>

/* TODO
 * - Ask to load file again if it was changed outside of editor
 * - save file -> put to menu + tool bar
 * - save new file
 * - Set from options:
 *      - font size, family, color, background
 * - syntax highlighting
 * - intellisense
 * */

class FileView : public QMdiSubWindow
{
    Q_OBJECT
protected:
    QString filePath;
    QTextEdit& editor;

public:
    explicit FileView(QWidget *parent = nullptr);

    void setFilePath(const QString& path);
    void setText(const QString& t);
    void saveFile();

protected:
    void keyPressEvent(QKeyEvent* ke);

public slots:
    void fileContentModified(bool changed);
};


class FileManager
{
public:
    static void openFileInEditor(QString& path);
};


