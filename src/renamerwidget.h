#ifndef RENAMERWIDGET_H
#define RENAMERWIDGET_H

#include <QWidget>
#include <QMap>
#include <QListWidget>
#include "settingsdialog.h"

namespace Ui {
class RenamerWidget;
}

class RenamerWidget : public QWidget
{
	Q_OBJECT

public:
	explicit RenamerWidget(QWidget *parent = nullptr);
	~RenamerWidget();

public slots:
	void listShows();
	void listShowContents();
	void listSeasonContents();

	void renameShow();
	void renameSeason();
	void renameSeasonShortcutButtons();
	void renameEpisodes();

	void moveSelectedFiles();
	void reorderSelectedEpisodes();
protected:
	void connectSignals();
	bool renameFileOrFolder(QString oldName, QString newName);
	bool moveFilesToFolder(QStringList files, QString folder);

	Ui::RenamerWidget *ui;
	SettingsDialog *settingsDialog;
};

#endif // RENAMERWIDGET_H
