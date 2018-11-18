#include "fileview.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QKeyEvent>
#include <QTextDocumentWriter>

#include <QFileDialog>

#include "mainwindow.h"
#include "fileedit.h"
#include "filesystem.h"
#include "linenumberarea.h"

#define NEW_FILE "{new file}"

FileView::FileView(QWidget *parent) : QWidget(parent)
{
    m_nameLabel = new QLabel(this);
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    m_layout = new QGridLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);

    QToolBar* toolbar = createToolbar();
    m_layout->addWidget(toolbar, 0, 0, 1, 2);

    m_editor = new FileEdit(this);
    setFocusProxy(m_editor);
    m_editor->installEventFilter(this);
    m_layout->addWidget(m_editor, 1, 1);

    m_lineNumberArea = new LineNumberArea(this, m_editor);
    m_layout->addWidget(m_lineNumberArea, 1, 0);

    connect(m_editor->document(), &QTextDocument::modificationChanged,
            this, &FileView::fileContentModified);
}

QToolBar* FileView::createToolbar()
{
    QToolBar* toolbar = new QToolBar("FileView", this);
    m_nameLabel->setText(NEW_FILE);
    toolbar->addWidget(m_nameLabel);
    QWidget *spacerWidget = new QWidget(this);
    spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolbar->addWidget(spacerWidget);
    toolbar->addAction("X", [this](){
        MainWindow::instance()->getDocumentManager()->closeFileView(this);
    });

    toolbar->setFixedHeight(20);

    return toolbar;
}

bool FileView::eventFilter(QObject *object, QEvent *event)
{
    auto type = event->type();
    if(type == QEvent::FocusIn ||
        type == QEvent::MouseButtonPress)
    {
        // TODO log MainWindow::instance()->getTerminalView()->showMessage(tr("last filview is %1").arg((long long)this));
        gotFocus();
    }
    return false;
}

void FileView::openFile(const QString& path)
{
    // If path is empty, we want to open a new file
    if(path.isEmpty())
    {
        m_nameLabel->setText(NEW_FILE);
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
    //Handle Ctrl+S as save file
    // TODO: make keybinding configurable from settings
    if(ke->key() == Qt::Key_S && ke->modifiers().testFlag(Qt::ControlModifier))
    {
        auto terminal = MainWindow::instance()->getTerminalView();
        terminal->showMessage(tr("Saving \"%1\"").arg(m_FilePath));
        ke->setAccepted(true);
        saveFile();
    } 
    else if(ke->key() == Qt::Key_D && ke->modifiers().testFlag(Qt::ControlModifier))
    {
        m_editor->fold();
    }
    else if(ke->key() == Qt::Key_F && ke->modifiers().testFlag(Qt::ControlModifier))
    {
        m_editor->unfold();
    }
    else
    {
        QWidget::keyPressEvent(ke);
    }
}

void FileView::saveFile()
{
    auto terminal = MainWindow::instance()->getTerminalView();
    if(m_FilePath == "" || !FS::doesFileExist(m_FilePath))
    {
        // Open save as dialog
        QFileDialog fileDialog(this, tr("Save as..."));
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);

        auto& state = MainWindow::instance()->getAppState();
        const QString& lastDir = state.getLastOpenedDirectory();
        fileDialog.setDirectory(lastDir);

        fileDialog.setDefaultSuffix("cpp");
        if (fileDialog.exec() != QDialog::Accepted)
            return;
        const QString path = QDir::toNativeSeparators(fileDialog.selectedFiles().first());
        setFilePath(path);

        state.setLastOpenedDirectory(FS::getDirectory(path).absolutePath());
        state.saveStateToDisk();
    }

    QTextDocumentWriter writer(m_FilePath);
    writer.setFormat("plaintext");
    bool success = writer.write(m_editor->document());
    if (success) {
        m_editor->document()->setModified(false);
        terminal->showMessage(tr("Saved \"%1\"").arg(m_FilePath));
    } else {
        terminal->showMessage(tr("Saving failed. Could not write to file \"%1\"")
                                 .arg(m_FilePath));
    }
}

void FileView::setFilePath(const QString &path)
{
    m_FilePath = path;
    m_editor->setFilePath(path);
    m_nameLabel->setText(path);
}

const QString& FileView::getFilePath()
{
    return m_FilePath;
}

void FileView::fileContentModified(bool changed)
{
    if(changed)
        m_nameLabel->setText(m_FilePath+"*");
    else
        m_nameLabel->setText(m_FilePath);
}

void FileView::setText(const QString& t)
{
    m_editor->setPlainText(t);
    m_editor->document()->setModified(false);
}
