#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QDialog>
#include <QLineEdit>

class SettingsView : public QDialog
{
	Q_OBJECT
private:
	QLineEdit* m_includeEdit;

public:
	SettingsView(QWidget* parent);

public slots:
	void accept() override;
	void includePathEditPushed();

};

#endif //SETTINGSVIEW_H