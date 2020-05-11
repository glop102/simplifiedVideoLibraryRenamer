#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include "glopConfig.h"

namespace Ui {
class Settings;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = nullptr);
	~SettingsDialog();

	GlopConfig::Settings settings;
	QString operator[](QString);
public slots:
	void open();
	void accept();
	void updateExampleRename();
	void openFileDialog();
private:
	Ui::Settings *ui;
};

#endif // SETTINGS_H
