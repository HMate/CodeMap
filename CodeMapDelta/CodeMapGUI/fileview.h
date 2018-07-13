#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QString>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QToolBar>

/* TODO
 * - Ask to load file again if it was changed outside of editor
 * - save file -> put to menu + tool bar
 * - save new file
 * - Set from options:
 *      - font size, family, color, background
 * - syntax highlighting
 * - intellisense
 * */

class FileView : public QWidget
{
    Q_OBJECT

    QGridLayout* layout;
    QLabel* nameLabel;
protected:
    QString filePath;
    QTextEdit& editor;

public:
    explicit FileView(QWidget *parent = nullptr);

    void setFilePath(const QString& path);
    const QString& getFilePath();
    void setText(const QString& t);

    void saveFile();
    void openFile(const QString &path);
protected:
    QToolBar* createToolbar();
    void keyPressEvent(QKeyEvent* ke);

public slots:
    void fileContentModified(bool changed);
    void closeView();
};

#endif // FILEVIEW_H
