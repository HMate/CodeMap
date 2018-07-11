#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QString>
#include <QTextEdit>
#include <QGridLayout>

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
protected:
    QString filePath;
    QTextEdit& editor;

public:
    explicit FileView(QWidget *parent = nullptr);

    void setFilePath(const QString& path);
    void setText(const QString& t);
    void saveFile();

    void openFile(const QString &path);
protected:
    void keyPressEvent(QKeyEvent* ke);

public slots:
    void fileContentModified(bool changed);
    void closeView();
};

#endif // FILEVIEW_H
