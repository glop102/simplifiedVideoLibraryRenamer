#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);
	createLayouts();
	createWidgets();
}

MainWindow::~MainWindow()
{

}

void MainWindow::createLayouts(){
	mainLayout = new QGridLayout();
	centralWidget->setLayout(mainLayout);

	seasonButtonsLayout = new QGridLayout();
	mainLayout->addLayout(seasonButtonsLayout,2,2); // under the season list

	overrideSettingsLayout = new QVBoxLayout();
	mainLayout->addLayout(overrideSettingsLayout,1,4,2,1,Qt::AlignTop); // slignment pushs all widget to the top instead of spacing them out vertically

	mainLayout->setColumnStretch(1,1);
	mainLayout->setColumnStretch(2,1);
	mainLayout->setColumnStretch(3,1);
	mainLayout->setColumnStretch(4,0); // keep the right column using the minimum amount of space

	mainLayout->setRowStretch(1,1);
	mainLayout->setRowStretch(2,0);
	//the other rows default to 0, so just image they are here
}

void MainWindow::createWidgets(){
	//==============================================================================================
	//the lists that we put some info into
	showList = new QListWidget();
	seasonList = new QListWidget();
	episodeList = new QListWidget();
	mainLayout->addWidget(showList,1,1,2,1);
	mainLayout->addWidget(seasonList,1,2,1,1); // only spans 1 row to accomodate the extra buttons under it
	mainLayout->addWidget(episodeList,1,3,2,1);

	//==============================================================================================
	//override settings on the right side of the window
	settingsButton = new QPushButton("Settings");
	overrideSettingsWidgets.showNameEdit = new QLineEdit();
	overrideSettingsWidgets.seasonNumberEdit = new QLineEdit();
	overrideSettingsWidgets.episodeStartingNumberEdit = new QLineEdit();
	overrideSettingsWidgets.moveEpisode_Up_button = new QPushButton("Move Up");
	overrideSettingsWidgets.moveEpisode_Ignore_button = new QPushButton("Ignore");
	overrideSettingsWidgets.moveEpisode_Down_button = new QPushButton("Move Down");

	overrideSettingsLayout->addWidget(settingsButton);
	overrideSettingsLayout->addWidget( new QLabel("Overriding Parameters") );
	overrideSettingsLayout->addWidget( new QLabel("Show Name") );
	overrideSettingsLayout->addWidget(overrideSettingsWidgets.showNameEdit);
	overrideSettingsLayout->addWidget( new QLabel("Season Number") );
	overrideSettingsLayout->addWidget(overrideSettingsWidgets.seasonNumberEdit);
	overrideSettingsLayout->addWidget( new QLabel("Starting Episode Number") );
	overrideSettingsLayout->addWidget(overrideSettingsWidgets.episodeStartingNumberEdit);
	overrideSettingsLayout->addWidget(overrideSettingsWidgets.moveEpisode_Up_button);
	overrideSettingsLayout->addWidget(overrideSettingsWidgets.moveEpisode_Ignore_button);
	overrideSettingsLayout->addWidget(overrideSettingsWidgets.moveEpisode_Down_button);

	//==============================================================================================
	//the season hot-buttons under the season list
	seasonButtonWidgets.season0Button = new QPushButton("S0");
	seasonButtonWidgets.season1Button = new QPushButton("S1");
	seasonButtonWidgets.season2Button = new QPushButton("S2");
	seasonButtonWidgets.season3Button = new QPushButton("S3");
	seasonButtonWidgets.season4Button = new QPushButton("S4");
	seasonButtonWidgets.season5Button = new QPushButton("S5");
	seasonButtonWidgets.extraButton = new QPushButton("extra");
	seasonButtonWidgets.seasonOtherButton = new QPushButton("Season");
	seasonButtonWidgets.seasonOtherEntry = new QLineEdit();

	seasonButtonsLayout->addWidget(seasonButtonWidgets.season1Button,1,1,Qt::AlignTop);
	seasonButtonsLayout->addWidget(seasonButtonWidgets.season2Button,1,2,Qt::AlignTop);
	seasonButtonsLayout->addWidget(seasonButtonWidgets.season3Button,1,3,Qt::AlignTop);
	seasonButtonsLayout->addWidget(seasonButtonWidgets.season4Button,1,4,Qt::AlignTop);
	seasonButtonsLayout->addWidget(seasonButtonWidgets.season5Button,1,5,Qt::AlignTop);
	seasonButtonsLayout->addWidget(seasonButtonWidgets.season0Button,2,1,Qt::AlignTop);
	seasonButtonsLayout->addWidget(seasonButtonWidgets.extraButton,2,2,1,2,Qt::AlignTop);
	seasonButtonsLayout->addWidget(seasonButtonWidgets.seasonOtherButton,3,1,1,2,Qt::AlignTop);
	seasonButtonsLayout->addWidget(seasonButtonWidgets.seasonOtherEntry,3,3,1,3,Qt::AlignTop);

	//==============================================================================================
	//the items under the lists
	belowListWidgets.showNameEntry = new QLineEdit();
	belowListWidgets.showRenameButton = new QPushButton("Rename");
	belowListWidgets.seasonNameEntry = new QLineEdit();
	belowListWidgets.seasonRenameButton = new QPushButton("Rename");
	belowListWidgets.moveToSeasonButton = new QPushButton("Move Left");

	renameSeasonButton = new QPushButton("Rename Season");
	moveShowToLibraryButton = new QPushButton("Move To Library");

	mainLayout->addWidget(belowListWidgets.showNameEntry,3,1);
	mainLayout->addWidget(belowListWidgets.showRenameButton,4,1);
	mainLayout->addWidget(belowListWidgets.seasonNameEntry,3,2);
	mainLayout->addWidget(belowListWidgets.seasonRenameButton,4,2);
	mainLayout->addWidget(belowListWidgets.moveToSeasonButton,3,3);

	mainLayout->addWidget(renameSeasonButton,3,4);
	mainLayout->addWidget(moveShowToLibraryButton,4,4);
}
