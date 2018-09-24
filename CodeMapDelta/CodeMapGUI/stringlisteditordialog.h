#ifndef STRINGLISTEDITORDIALOG_H
#define STRINGLISTEDITORDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QListWidget>

class StringListEditorDialog : public QDialog
{
	Q_OBJECT 

	QStringList m_list;
	QListWidget* m_listView = nullptr;

public:
	StringListEditorDialog(QWidget* parent, const QStringList& initialList);

	QStringList getStringList();

public slots:
	void addPath();
	void editPath();
	void deletePath();
};

#endif // STRINGLISTEDITORDIALOG_H