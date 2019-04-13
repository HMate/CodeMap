#include "StringListEditorDialog.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QPushButton>
#include <QFileDialog>

#include "MainWindow.h"

StringListEditorDialog::StringListEditorDialog(QWidget* parent, const QStringList& initialList)
{
    // TODO: Should we restrict how many characters/rows can be in the editor?

    setMinimumWidth(300);
    setMinimumHeight(500);

    m_listView = new QListWidget(this);

    m_listView->addItems(initialList);
    
    connect(m_listView, &QListWidget::itemClicked, this, &StringListEditorDialog::itemClicked);
    connect(m_listView, &QListWidget::itemDoubleClicked, this, &StringListEditorDialog::itemDoubleClicked);

    QPushButton* addButton = new QPushButton(tr("Add"), this);
    QPushButton* editButton = new QPushButton(tr("Edit"), this);
    QPushButton* deleteButton = new QPushButton(tr("Delete"), this);

    connect(addButton, &QPushButton::clicked, this, &StringListEditorDialog::addPath);
    connect(editButton, &QPushButton::clicked, this, &StringListEditorDialog::editPath);
    connect(deleteButton, &QPushButton::clicked, this, &StringListEditorDialog::deletePath);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto layout = new QGridLayout(this);
    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 1);
    layout->setRowStretch(2, 99);
    layout->setRowStretch(3, 0);

    layout->addWidget(m_listView, 0, 0, 3, 1);
    layout->addWidget(addButton, 0, 1, 1, 1, Qt::AlignBottom);
    layout->addWidget(editButton, 1, 1, 1, 1, Qt::AlignCenter);
    layout->addWidget(deleteButton, 2, 1, 1, 1, Qt::AlignTop);
    layout->addWidget(buttonBox, 3, 0, 1, 2);
}

void StringListEditorDialog::itemClicked(QListWidgetItem *item)
{
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    m_listView->editItem(item);
}

void StringListEditorDialog::itemDoubleClicked(QListWidgetItem *item)
{
    m_listView->setCurrentItem(item);
    editPath();
}

void StringListEditorDialog::addPath()
{
    // TODO: For this class to be general, this should be an argument if its file/directory/both picker
    // Currently this is more of a DirectoryListEditorDialog
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::FileMode::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);

    // TODO: cache last path opened here and open dialog there?
    auto& state = MainWindow::instance()->getAppState();
    const QString& lastDir = state.getLastOpenedDirectory();
    if(QDir(lastDir).exists())
        dialog.setDirectory(lastDir);
    if(dialog.exec() == QDialog::Accepted)
    {
        const QStringList& directories = dialog.selectedFiles();
        m_listView->addItems(directories);
    }
}

void StringListEditorDialog::editPath()
{
    auto it = m_listView->currentItem();
    
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::FileMode::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);

    const QString& lastDir = it->text();
    if(QDir(lastDir).exists())
        dialog.setDirectory(lastDir);
    if(dialog.exec() == QDialog::Accepted)
    {
        const QStringList& directories = dialog.selectedFiles();
        it->setText(directories[0]);
    }
}

void StringListEditorDialog::deletePath()
{
    auto it = m_listView->currentItem();
    auto detached = m_listView->takeItem(m_listView->row(it));
    delete detached;
}

QStringList StringListEditorDialog::getStringList()
{
    QStringList result;
    for(auto i = 0; i < m_listView->count(); i++)
    {
        result.append(m_listView->item(i)->text());
    }
    return result;
}