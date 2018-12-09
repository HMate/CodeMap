#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QToolBar>

#include <QTextCursor>

class LineNumberArea;
class EditorFoldingArea;
class FileEdit;

/* TODO
 * - Ask to load file again if it was changed outside of editor
 * - save file -> put to menu + tool bar
 * - create/save as new file 
        - saveFile can handle it, just need to bind it to a command
 * - Set from options:
 *      - font size, family, color, background, word wrap
 * - Key rebinding
 * - syntax highlighting
 * - intellisense
 * - Code navigation
     - Find in file/project/all open files/file in cwd
     - Go to declaration/implementation
     - switch header/cpp
 * */

/*
Wrapper class around the file editor, that provides an interface for others to use the editor
*/
class FileView : public QWidget
{
    Q_OBJECT

    QGridLayout* m_layout = nullptr;

    LineNumberArea* m_lineNumberArea = nullptr;
    EditorFoldingArea* m_foldingArea = nullptr;
    FileEdit* m_editor = nullptr;

    QLabel* m_nameLabel = nullptr;
    QString m_filePath;

public:
    explicit FileView(QWidget *parent = nullptr);

    FileEdit* getEditor() { return m_editor; }

    void setFilePath(const QString& path);
    const QString& getFilePath();
    void setText(const QString& t);

    void setIncludeCollapsers();

    void saveFile();
    void openFile(const QString &path);

    bool eventFilter(QObject *object, QEvent *event);
protected:
    QToolBar* createToolbar();
    void keyPressEvent(QKeyEvent* ke);

signals:
    void gotFocus();

public slots:
    void fileContentModified(bool changed);
    void closeView();
};

#endif // FILEVIEW_H
