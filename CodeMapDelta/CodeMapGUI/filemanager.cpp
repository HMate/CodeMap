#include "filemanager.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMdiSubWindow>
#include <QKeyEvent>
#include <QTextDocumentWriter>

#include "mainwindow.h"


void FileManager::openFileInEditor(QString& path)
{
    const MainWindow* mainW = MainWindow::instance();
    auto docManager = mainW->getDocumentManager();
    mainW->getTerminalView()->registerCommand("Opening: " + path);

    // TODO: Show error, or log it somewhere if not exists?
    if(!QFile::exists(path))
        return;

    FileView* openedFile = new FileView(docManager);
    openedFile->setFilePath(path);
    QMdiSubWindow* subwindow = docManager->addSubWindow(openedFile);
    subwindow->setAttribute(Qt::WA_DeleteOnClose, true);
    subwindow->show();

    QFile file(path);

    // TODO: Show error, or log it somewhere if not exists?
    if(!file.open(QFile::ReadOnly | QFile::Text))
        return;

    QTextStream ReadFile(&file);
    openedFile->setText(ReadFile.readAll());
}

FileView::FileView(QWidget *parent) : QMdiSubWindow(parent), editor(*new QTextEdit(this))
{
    editor.setVisible(true);
    editor.setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
    editor.setFont(QFont("Consolas"));

    setWidget(&editor);

    connect(editor.document(), &QTextDocument::modificationChanged, this, &FileView::fileContentModified);

    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
}

void FileView::keyPressEvent(QKeyEvent* ke)
{
    if(ke->key() == Qt::Key_S && ke->modifiers().testFlag(Qt::ControlModifier))
    {
        auto terminal = MainWindow::instance()->getTerminalView();
        terminal->registerCommand("Saving " + filePath);
        ke->setAccepted(true);
        saveFile();
    }
    else
    {
        QMdiSubWindow::keyPressEvent(ke);
    }
}

void FileView::saveFile()
{
    auto terminal = MainWindow::instance()->getTerminalView();
    if(filePath == "")
    {
        // TODO: Open save as dialog
        /*
        QFileDialog fileDialog(this, tr("Save as..."));
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);
        QStringList mimeTypes;
        mimeTypes << "application/vnd.oasis.opendocument.text" << "text/html" << "text/plain";
        fileDialog.setMimeTypeFilters(mimeTypes);
        fileDialog.setDefaultSuffix("odt");
        if (fileDialog.exec() != QDialog::Accepted)
            return false;
        const QString fn = fileDialog.selectedFiles().first();
        setCurrentFileName(fn);
        */
    }

    QTextDocumentWriter writer(filePath);
    bool success = writer.write(editor.document());
    if (success) {
        editor.document()->setModified(false);
        terminal->registerCommand(tr("Saved \"%1\"").arg(QDir::toNativeSeparators(filePath)));
    } else {
        terminal->registerCommand(tr("Saving failed. Could not write to file \"%1\"")
                                 .arg(QDir::toNativeSeparators(filePath)));
    }
}

void FileView::setFilePath(const QString &path)
{
    filePath = path;
    setWindowTitle("[*]" + filePath);
}

void FileView::fileContentModified(bool changed)
{
    setWindowModified(changed);
}

void FileView::setText(const QString& t)
{
    editor.setText(t);
    editor.document()->setModified(false);
    setWindowModified(false);
}
