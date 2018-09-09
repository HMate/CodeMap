#include "fileview.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QKeyEvent>
#include <QTextDocumentWriter>
#include <QGridLayout>

#include "mainwindow.h"

FileView::FileView(QWidget *parent) : QWidget(parent)
{
    nameLabel = new QLabel(this);
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    QToolBar* toolbar = createToolbar();
    layout->addWidget(toolbar, 0, 0);

    editor = new FileEdit(this);
    setFocusProxy(editor);
    layout->addWidget(editor, 1, 0);

    connect(editor->document(), &QTextDocument::modificationChanged,
            this, &FileView::fileContentModified);
}

QToolBar* FileView::createToolbar()
{
    QToolBar* toolbar = new QToolBar("FileView", this);
    nameLabel->setText("{new file}");
    toolbar->addWidget(nameLabel);
    QWidget *spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacerWidget);
    toolbar->addAction("X", [this](){
        MainWindow::instance()->getDocumentManager()->closeFileView(filePath);
    });

    toolbar->setFixedHeight(20);

    return toolbar;
}

void FileView::openFile(const QString& path)
{
    // If path is empty, we want to open a new file
    if(path.isEmpty())
    {
        nameLabel->setText("{new file}");
        return;
    }

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

void FileView::closeView()
{
    this->close();
}

void FileView::keyPressEvent(QKeyEvent* ke)
{
    if(ke->key() == Qt::Key_S && ke->modifiers().testFlag(Qt::ControlModifier))
    {
        auto terminal = MainWindow::instance()->getTerminalView();
        terminal->showMessage(tr("Saving \"%1\"").arg(filePath));
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
    writer.setFormat("plaintext");
    bool success = writer.write(editor->document());
    if (success) {
        editor->document()->setModified(false);
        terminal->showMessage(tr("Saved \"%1\"").arg(filePath));
    } else {
        terminal->showMessage(tr("Saving failed. Could not write to file \"%1\"")
                                 .arg(filePath));
    }
}

void FileView::setFilePath(const QString &path)
{
    filePath = path;
    nameLabel->setText(path);
}

const QString& FileView::getFilePath()
{
    return filePath;
}

void FileView::fileContentModified(bool changed)
{
    if(changed)
        nameLabel->setText(filePath+"*");
    else
        nameLabel->setText(filePath);
}

void FileView::setText(const QString& t)
{
    editor->setText(t);
    editor->document()->setModified(false);
}
