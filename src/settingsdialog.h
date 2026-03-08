#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include "glopConfig.h"

namespace Ui {
class Settings;
}

#define DEFAULT_DELETE_FILENAMES "RARBG.txt;Torrent downloaded from Demonoid.com.txt;RARBG.com.txt;Info.txt;"

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

	void modifyAutoDeleteFilenames();
private:
	Ui::Settings *ui;
};

#endif // SETTINGS_H
