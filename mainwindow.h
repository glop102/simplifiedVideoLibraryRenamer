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

class MainWindow : public QMainWindow
{
	Q_OBJECT
	QWidget *centralWidget;
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	void createLayouts(); // creates the layouts to put the widgets into
	void createWidgets(); // handles setting up the widgets into the layouts

private:
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
		QLineEdit *showNameEdit,*seasonNumberEdit,*episodeStartingNumberEdit;
		QPushButton *moveEpisode_Up_button;
		QPushButton *moveEpisode_Ignore_button;
		QPushButton *moveEpisode_Down_button;
	}overrideSettingsWidgets;

	struct{
		QPushButton *season1Button,*season2Button,*season3Button,*season4Button,*season5Button;
		QPushButton *season0Button,*extraButton;
		QPushButton *seasonOtherButton;
		QLineEdit *seasonOtherEntry; // user puts in the different season number here and presses the above button
	}seasonButtonWidgets;

	QPushButton *renameSeasonButton; // renames all episodes in the selected season based on settings
	QPushButton *moveShowToLibraryButton; // moves the entire show into the library folder specified
};

#endif // MAINWINDOW_H
