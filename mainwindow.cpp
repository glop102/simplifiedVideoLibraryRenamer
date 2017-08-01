#include "mainwindow.h"

QTextStream& qStdOut() {
	static QTextStream ts( stdout );
	return ts;
	//usage
	//qStdOut() << thing;
	//qStdOut().flush();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);
	createLayouts();
	createWidgets();
	createConnections();

	GlopConfig::Settings s = GlopConfig::ParseFile("settings.conf");
	importLocation = QString::fromStdString(s.values["import location"]);
	libraryLocation = QString::fromStdString(s.values["library location"]);

	listShows(importLocation);
}

MainWindow::~MainWindow(){
	// screw deleting all the widgets, they are deleted automatically for us
}

//========================================================================================================================================================================================
void MainWindow::listShows(QString location){
	showList->clear(); // make it empty so we only need to worry about adding items
	seasonList->clear();
	episodeList->clear();

	QFileInfo info(location);
	if(!info.isDir()) return; // cant list contents unless is a folder

	QDir loc(location);
	if(!loc.exists()){
		// TODO - OPEN SETTINGS WINDOW
		qStdOut() << "unable to open location : " << location << "\n" ;
		qStdOut().flush();
		return;
	}
	QStringList list = loc.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(int x=0; x<list.length(); x++){
		showList->addItem(list[x]);
	}

	list = loc.entryList(QDir::Files);
	for(int x=0; x<list.length(); x++){
		QListWidgetItem *temp = new QListWidgetItem(list[x]);
		temp->setBackground(Qt::red);
		showList->addItem(temp);
	}
}

void MainWindow::listShowContents(QString location){
	seasonList->clear(); // make it empty so we only need to worry about adding items
	episodeList->clear();

	QFileInfo info(location);
	if(!info.isDir()) return; // cant list contents unless is a folder

	QDir loc(location);
	if(!loc.exists()){
		qStdOut() << "unable to open location : " << location << "\n" ;
		qStdOut().flush();
		return;
	}
	QStringList list = loc.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(int x=0; x<list.length(); x++){
		seasonList->addItem(list[x]);
	}

	list = loc.entryList(QDir::Files);
	for(int x=0; x<list.length(); x++){
		QListWidgetItem *temp = new QListWidgetItem(list[x]);
		temp->setBackground(Qt::red);
		seasonList->addItem(temp);
	}
}

void MainWindow::listSeasonContents(QString location){
	episodeList->clear(); // make it empty so we only need to worry about adding items

	QFileInfo info(location);
	if(!info.isDir()) return; // cant list contents unless is a folder

	QDir loc(location);
	if(!loc.exists()){
		qStdOut() << "unable to open location : " << location << "\n" ;
		qStdOut().flush();
		return;
	}
	QStringList list = loc.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(int x=0; x<list.length(); x++){
		QListWidgetItem *temp = new QListWidgetItem(list[x]);
		temp->setBackground(Qt::red);
		episodeList->addItem(temp);
	}

	list = loc.entryList(QDir::Files);
	for(int x=0; x<list.length(); x++){
		episodeList->addItem(list[x]);
	}
}

void MainWindow::showListClicked(QListWidgetItem *current){
	if(current == NULL) return;
	listShowContents(importLocation+"/"+
					 current->text()
					 );
	belowListWidgets.showNameEntry->setText(current->text()); // make it easy to rename a show
	belowListWidgets.seasonNameEntry->setText("");

	//populate the overrides with save defaults
	overrideSettingsWidgets.showNameEdit->setText(current->text());
	overrideSettingsWidgets.seasonNumberEdit->setValue(1);
	overrideSettingsWidgets.episodeStartingNumberEdit->setValue(1);
}
void MainWindow::seasonListClicked(QListWidgetItem *current){
	if(current == NULL) return;
	if(showList->currentItem() == NULL) return;
	listSeasonContents(importLocation+"/"+
					   showList->currentItem()->text()+"/"+
					   current->text()
					   );
	belowListWidgets.seasonNameEntry->setText(current->text()); // make it easy to rename a season

	// now parse the name and see if we understand the season number
	// and then put that number into the override box
	QString name = current->text().toLower();
	QStringList parts = name.split(" "); // split along spaces
	if( parts.size()==2 && parts[0]=="season" ){ // we only understand the format of "season #"
		int seasonNumber = parts[1].toInt(); // assume the second entry is the number
		overrideSettingsWidgets.seasonNumberEdit->setValue(seasonNumber); // set it without bothering if was a valid conversion - returns 0 if it failed which is fine
	}
}

void MainWindow::renameShow(){
	if(showList->currentItem() == NULL) return;
	QString oldName, newName;
	int currentIndex;
	oldName = importLocation + "/" + showList->currentItem()->text();
	newName = importLocation + "/" + belowListWidgets.showNameEntry->text();
	currentIndex = showList->currentRow();

	if(oldName == newName) return;

	renameFileOrFolder(oldName,newName);

	listShows(importLocation);
	showList->setCurrentRow(currentIndex%showList->count()); // ensure that one of the items in the list is counted as activated
}

void MainWindow::renameSeason(){
	if(showList->currentItem() == NULL) return;
	if(seasonList->currentItem() == NULL) return;
	QString oldName, newName;
	int currentIndex;
	oldName = importLocation + "/" + showList->currentItem()->text() + "/" + seasonList->currentItem()->text();
	newName = importLocation + "/" + showList->currentItem()->text() + "/" + belowListWidgets.seasonNameEntry->text();
	currentIndex = seasonList->currentRow();

	if(oldName == newName) return;

	renameFileOrFolder(oldName,newName);

	listShowContents(importLocation+"/"+showList->currentItem()->text());
	seasonList->setCurrentRow(currentIndex%seasonList->count()); // ensure that one of the items in the list is counted as activated
}

void MainWindow::addSeasonPressed(){
	if(showList->currentItem() == NULL) return;
	//first lets find out who made the event
	QObject *sender = QObject::sender();

	//Now figure out what to do
	QString newFolder;
	if(sender == seasonButtonWidgets.season0Button)
		newFolder = "Season 0";
	if(sender == seasonButtonWidgets.season1Button)
		newFolder = "Season 1";
	if(sender == seasonButtonWidgets.season2Button)
		newFolder = "Season 2";
	if(sender == seasonButtonWidgets.season3Button)
		newFolder = "Season 3";
	if(sender == seasonButtonWidgets.season4Button)
		newFolder = "Season 4";
	if(sender == seasonButtonWidgets.season5Button)
		newFolder = "Season 5";
	if(sender == seasonButtonWidgets.seasonOtherButton)
		newFolder = "Season " + QString::number(seasonButtonWidgets.seasonOtherEntry->value()) ;
	if(sender == seasonButtonWidgets.extraButton)
		newFolder = "Extra";

	if(newFolder.length() == 0){
		qWarning("addSeasonPressed() : unknown and unhandeled callback to here\nDid you forget to add something?\n");
		return;
	}

	//Now we get a list of filenames from the season list that are to be moved
	QList<QListWidgetItem*> names = seasonList->selectedItems();
	QStringList paths;
	QString showName = showList->currentItem()->text();
	if(names.length()==0) return; // dont make empty folders
	for(int x=0; x<names.length(); x++){
		paths.push_back(importLocation+"/"+showName+"/"+names[x]->text());
	}
	moveFilesToFolder(paths,importLocation+"/"+showName+"/"+newFolder);
	listShowContents(importLocation+"/"+showName);
}

//========================================================================================================================================================================================

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
	showList->setSelectionMode(QAbstractItemView::ExtendedSelection);   // let the user select multiple items in the list
	seasonList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	episodeList->setSelectionMode(QAbstractItemView::ExtendedSelection);

	//==============================================================================================
	//override settings on the right side of the window
	settingsButton = new QPushButton("Settings");
	overrideSettingsWidgets.showNameEdit = new QLineEdit();
	overrideSettingsWidgets.seasonNumberEdit = new QSpinBox();
	overrideSettingsWidgets.episodeStartingNumberEdit = new QSpinBox();
	overrideSettingsWidgets.moveEpisode_Up_button = new QPushButton("Move Up");
	overrideSettingsWidgets.moveEpisode_Ignore_button = new QPushButton("Ignore");
	overrideSettingsWidgets.moveEpisode_Down_button = new QPushButton("Move Down");

	overrideSettingsWidgets.episodeStartingNumberEdit->setMaximum(10000); // just in case we ever have 10000 episodes in a season

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
	seasonButtonWidgets.seasonOtherEntry = new QSpinBox();

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

void MainWindow::createConnections(){
	connect(showList,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(showListClicked(QListWidgetItem*)) );
	connect(seasonList,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(seasonListClicked(QListWidgetItem*)) );

	connect(belowListWidgets.showRenameButton,SIGNAL(clicked(bool)),this,SLOT(renameShow()) );
	connect(belowListWidgets.showNameEntry,SIGNAL(returnPressed()),this,SLOT(renameShow()) );
	connect(belowListWidgets.seasonRenameButton,SIGNAL(clicked(bool)),this,SLOT(renameSeason()) );
	connect(belowListWidgets.seasonNameEntry,SIGNAL(returnPressed()),this,SLOT(renameSeason()) );

	connect(seasonButtonWidgets.extraButton,SIGNAL(clicked(bool)),this,SLOT(addSeasonPressed()) );
	connect(seasonButtonWidgets.season0Button,SIGNAL(clicked(bool)),this,SLOT(addSeasonPressed()) );
	connect(seasonButtonWidgets.season1Button,SIGNAL(clicked(bool)),this,SLOT(addSeasonPressed()) );
	connect(seasonButtonWidgets.season2Button,SIGNAL(clicked(bool)),this,SLOT(addSeasonPressed()) );
	connect(seasonButtonWidgets.season3Button,SIGNAL(clicked(bool)),this,SLOT(addSeasonPressed()) );
	connect(seasonButtonWidgets.season4Button,SIGNAL(clicked(bool)),this,SLOT(addSeasonPressed()) );
	connect(seasonButtonWidgets.season5Button,SIGNAL(clicked(bool)),this,SLOT(addSeasonPressed()) );
	connect(seasonButtonWidgets.seasonOtherButton,SIGNAL(clicked(bool)),this,SLOT(addSeasonPressed()) );
}

void MainWindow::renameFileOrFolder(QString oldName, QString newName){
	QFileInfo source(oldName);
	QFileInfo dest(newName);
	if(source.isDir()){
		if(!dest.exists())
			QDir().rename(oldName,newName);
		else{
			//merge the folders
			QDir sd(oldName); // source directory
			QStringList list = sd.entryList(QDir::NoDotAndDotDot|QDir::Files|QDir::Dirs);
			for(int x=0; x<list.length(); x++){
				sd.rename(oldName+"/"+list[x],newName+"/"+list[x]); // move the files into the new folder
			}
			sd.rmdir(oldName); // safely fails if not empty
		}
	}else{
		if(!dest.exists())
			QFile::rename(oldName,newName);
		//else do nothing
	}
}

void MainWindow::moveFilesToFolder(QStringList files, QString folder){
	QFileInfo destFolder(folder);
	if(!destFolder.exists()){
		QDir().mkpath(folder); // make the folder
	}else if(destFolder.isFile())
		return; // cant move files into a file :P

	for(int x=0; x<files.length(); x++){
		QFileInfo source(files[x]);
		renameFileOrFolder(files[x],folder+"/"+source.fileName()); // move the file to the new location - does merge folders
	}
}
