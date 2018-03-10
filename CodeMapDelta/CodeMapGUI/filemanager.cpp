#include "filemanager.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QKeyEvent>
#include <QTextDocumentWriter>
#include <QGridLayout>

#include "mainwindow.h"

FileView::FileView(QWidget *parent) : QWidget(parent), editor(*new QTextEdit(this))
{

    layout = new QGridLayout(this);
    layout->setMargin(0);

    editor.setVisible(true);
    editor.setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
    editor.setFont(QFont("Consolas"));
    layout->addWidget(&editor);

    //setWidget(&editor);

    connect(editor.document(), &QTextDocument::modificationChanged,
            this, &FileView::fileContentModified);

    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
}

void FileView::openFile(const QString& path)
{
    // TODO: Show error, or log it somewhere if not exists?
    if(!QFile::exists(path))
        return;

    setFilePath(path);

    QFile file(path);

    // TODO: Show error, or log it somewhere if not exists?
    if(!file.open(QFile::ReadOnly | QFile::Text))
        return;

    QTextStream ReadFile(&file);
    setText(ReadFile.readAll());
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
        QWidget::keyPressEvent(ke);
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
