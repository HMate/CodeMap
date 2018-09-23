#include "settingsview.h"

#include <QLabel>
#include <QDialogButtonBox>
#include <QGridLayout>

#include "mainwindow.h"

SettingsView::SettingsView(QWidget* parent) : QDialog(parent)
{
	auto& settings = MainWindow::instance()->getAppState().settings();

	m_includeEdit = new QLineEdit(settings.globalIncludes.join(';'), this);

	auto includeLabel = new QLabel(tr("&Include Paths:"));
	includeLabel->setBuddy(m_includeEdit);

	auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	auto layout = new QGridLayout(this);
	layout->addWidget(includeLabel, 0, 0);
	layout->addWidget(m_includeEdit, 0, 1);
	layout->addWidget(buttonBox, 1, 0, 1, 2);
}

void SettingsView::accept()
{
	// TODO: Settings view should apply settings instantly, not only when accepting
	// and reset it back to previous settings on cancel.
	// TODO: Editor / filepicker for paths, validate paths
	auto includes = m_includeEdit->text();
	auto& list = includes.split(';', QString::SplitBehavior::SkipEmptyParts);
	MainWindow::instance()->getAppState().settings().globalIncludes = list;

	QDialog::accept();
}

SettingsView::~SettingsView()
{
}