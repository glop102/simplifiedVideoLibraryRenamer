#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QString>
#include <QDir>
#include <QTextStream>
#include <QBrush>
#include <QColor>
#include <QFileInfo>

#include "glopConfig.h"

/*
 * Asumptions Made
 *  - the currentItem in any list is the exact text that is on the filesystem
 *  - using the text from above properly directs QDir and QFile to the different locations
 *     - ex using the showList currentItem text is the name of the folder to list the contents for
*/

class MainWindow : public QMainWindow
{
	Q_OBJECT
	QWidget *centralWidget;
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	void createLayouts(); // creates the layouts to put the widgets into
	void createWidgets(); // handles setting up the widgets into the layouts
	void createConnections(); //connects the slots and signals needed

	void listShows(QString location);
	void listShowContents(QString location);
	void listSeasonContents(QString location);
public slots:
	// do the housekeeping of making sure the lists are properly populated
	// and that the different entry fields are properly populated
	void showListClicked(QListWidgetItem * current);
	void seasonListClicked(QListWidgetItem * current);

	void renameShow();	// called when the rename buttons are pressed
	void renameSeason();

	void addSeasonPressed();

private: // widgets
	QGridLayout* mainLayout; // counting from 1 and up for rows and columns
	QGridLayout* seasonButtonsLayout; // under the list for the season
	QVBoxLayout* overrideSettingsLayout; //on the right side

	QListWidget *showList,*seasonList,*episodeList;

	struct{
		QLineEdit *showNameEntry,*seasonNameEntry;
		QPushButton *showRenameButton,*seasonRenameButton,*moveToSeasonButton;
	}belowListWidgets;

	QPushButton *settingsButton;
	struct {
		QLineEdit *showNameEdit;
		QSpinBox *seasonNumberEdit,*episodeStartingNumberEdit;
		QPushButton *moveEpisode_Up_button;
		QPushButton *moveEpisode_Ignore_button;
		QPushButton *moveEpisode_Down_button;
	}overrideSettingsWidgets;

	struct{
		QPushButton *season1Button,*season2Button,*season3Button,*season4Button,*season5Button;
		QPushButton *season0Button,*extraButton;
		QPushButton *seasonOtherButton;
		QSpinBox *seasonOtherEntry; // user puts in the different season number here and presses the above button
	}seasonButtonWidgets;

	QPushButton *renameSeasonButton; // renames all episodes in the selected season based on settings
	QPushButton *moveShowToLibraryButton; // moves the entire show into the library folder specified

private: // state variables
	QString importLocation; // the folder that we have the lists show and we manipulate
	QString libraryLocation; // where we copy/move shows into from the import location

private:
	void renameFileOrFolder(QString oldName,QString newName); // trys to rename, but if is a folder, will try merging also
	void moveFilesToFolder(QStringList files,QString folder); // NOTE: also can pass in folders to the file-list
};

#endif // MAINWINDOW_H
