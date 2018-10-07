#ifndef FILEVIEW_H
#define FILEVIEW_H

#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QToolBar>

class FileEdit;

/* TODO
 * - Ask to load file again if it was changed outside of editor
 * - save file -> put to menu + tool bar
 * - save new file
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

class FileView : public QWidget
{
    Q_OBJECT

    QGridLayout* layout;
    FileEdit* editor;
    QLabel* nameLabel;
    QString m_FilePath;

public:
    explicit FileView(QWidget *parent = nullptr);

    void setFilePath(const QString& path);
    const QString& getFilePath();
    void setText(const QString& t);

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
