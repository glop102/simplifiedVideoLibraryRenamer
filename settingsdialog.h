#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QGridLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QFileDialog>
#include "glopConfig.h"

class SettingsDialog : public QDialog {
	Q_OBJECT
	GlopConfig::Settings newSettings;
	QGridLayout *layout;
	QLabel *seasonNumberLengthLabel,*episodeNumberLengthLabel;
	QSpinBox *seasonNumberLengthEntry,*episodeNumberLengthEntry;
	QLabel *exampleNamedEpisode;
	QPushButton *importLocationButton;
	QLineEdit *importLocationEntry;
	QPushButton *libraryLocationButton;
	QLineEdit *libraryLocationEntry;
	QPushButton *okButton,*cancelButton;
public:
	SettingsDialog(GlopConfig::Settings s, QWidget *parent=0);
	~SettingsDialog();
	GlopConfig::Settings getSettings();
public slots:
	void reject(); // called when the escape key is pressed - inherited from qdialog class
	void accept();
	void openFileDialog();
	void setNamingExample();

private:
	void createWidgets();
};

#endif // SETTINGSDIALOG_H
