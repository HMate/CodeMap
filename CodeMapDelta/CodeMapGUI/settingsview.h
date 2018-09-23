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
	~SettingsView();

public slots:
	void accept() override;

};

#endif //SETTINGSVIEW_H