#include "stringlisteditordialog.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QPushButton>
#include <QFileDialog>

#include "mainwindow.h"

StringListEditorDialog::StringListEditorDialog(QWidget* parent, const QStringList& initialList)
{
	// TODO: Should we restrict how many characters/rows can be in the editor?

	m_listView = new QListWidget(this);

	m_listView->addItems(initialList);

	QPushButton* addButton = new QPushButton(tr("Add"), this);
	QPushButton* editButton = new QPushButton(tr("Edit"), this);
	QPushButton* deleteButton = new QPushButton(tr("Delete"), this);

	connect(addButton, &QPushButton::clicked, this, &StringListEditorDialog::addPath);
	connect(editButton, &QPushButton::clicked, this, &StringListEditorDialog::editPath);
	connect(deleteButton, &QPushButton::clicked, this, &StringListEditorDialog::deletePath);

	auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	auto layout = new QGridLayout(this);
	layout->addWidget(m_listView, 0, 0, 3, 1);
	layout->addWidget(addButton, 0, 1, 1, 1);
	layout->addWidget(editButton, 1, 1, 1, 1);
	layout->addWidget(deleteButton, 2, 1, 1, 1);
	layout->addWidget(buttonBox, 3, 0, 1, 2);
}

void StringListEditorDialog::addPath()
{
	// TODO: For this class to be general, this should be an argument if its file/directory/both picker
	// Currently this is more of a DirectoryListEditorDialog
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::DirectoryOnly);
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
	dialog.setFileMode(QFileDialog::FileMode::DirectoryOnly);
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
	// TODO implement deleting the selected item from the list
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