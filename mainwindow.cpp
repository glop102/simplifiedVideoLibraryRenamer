#include "mainwindow.h"

//QTextStream& qStdOut() {
//	static QTextStream ts( stdout );
//	return ts;
//	//usage
//	//qStdOut() << thing;
//	//qStdOut().flush();
//}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
	centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);
	createLayouts();
	createWidgets();
	createConnections();
	loadConfig();

	listShows(importLocation);
}

MainWindow::~MainWindow(){
	// screw deleting all the widgets, they are deleted automatically for us

	saveConfig();
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
		qWarning(("unable to open location : " + location + "\n").toAscii() );
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
		qWarning(("unable to open location : " + location + "\n").toAscii() );
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
		qWarning(("unable to open location : " + location + "\n").toAscii() );
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

	//reset the episode start to 1
	overrideSettingsWidgets.episodeStartingNumberEdit->setValue(1);
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
void MainWindow::renameEpisodes(){
	//Sorry for the mess in here, but it is dense and terse
	//it is easy to understand if you take a minute of two and read it twice

	if(showList->currentItem() == NULL) return;
	if(seasonList->currentItem() == NULL) return;
	// first some basic information that takes nearly no processing
	QString showName = overrideSettingsWidgets.showNameEdit->text();
	int seasonNumber = overrideSettingsWidgets.seasonNumberEdit->value();
	int episodeNumber = overrideSettingsWidgets.episodeStartingNumberEdit->value();
	QString seasonPath = importLocation+"/"+showList->currentItem()->text()+"/"+seasonList->currentItem()->text();

	// loop over the filenames in the episode list - may not be in filesystem listing order
	// NOTE: we are incrementing TWO variables in here
	// - currentItem is which row in the list we are on
	// - episodeNumber is the episode number for the current file - starting at some number
	int numItems = episodeList->count();
	int currentItem = 0;
	while(currentItem<numItems){
		QString seasonString = QString::number(seasonNumber);
		while(seasonString.length()<seasonNumberLength) seasonString = "0"+seasonString;  // make it long enough as per our settings
		QString episodeString = QString::number(episodeNumber);
		while(episodeString.length()<episodeNumberLength) episodeString = "0"+episodeString;

		// does as the variable names suggest
		QFileInfo info(seasonPath+"/"+episodeList->item(currentItem)->text());
		QString filename;
		QString fileExtension = info.fileName().right(info.fileName().length() - info.fileName().lastIndexOf(".")); // filetype is to the right of the base name

		QString oldName = episodeList->item(currentItem)->text();
		QString newName = showName+".s"+seasonString+"e"+episodeString+fileExtension; // formated as per user settings

		renameFileOrFolder(seasonPath+"/"+oldName,seasonPath+"/"+newName);
		//qStdOut() << seasonPath+"/"+oldName+"\n" << seasonPath+"/"+newName+"\n";
		//qStdOut().flush();

		currentItem++;
		episodeNumber++;
	}

	listSeasonContents(seasonPath);
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
		qWarning("warning : addSeasonPressed() : unknown and unhandeled callback to here\nDid you forget to add something?\n");
		return;
	}

	//Now we get a list of filenames from the season list that are to be moved
	QList<QListWidgetItem*> names = seasonList->selectedItems();
	QStringList paths; // paths to files to move
	QString showName = showList->currentItem()->text();
	if(names.length()==0) return; // dont make empty folders
	for(int x=0; x<names.length(); x++){
		paths.push_back(importLocation+"/"+showName+"/"+names[x]->text());
	}
	moveFilesToFolder(paths,importLocation+"/"+showName+"/"+newFolder);
	listShowContents(importLocation+"/"+showName);
}
void MainWindow::generalMoveFilesPressed(){
	//first lets find out who made the event
	QObject *sender = QObject::sender();

	//Now figure out what to do
	if(sender == belowListWidgets.moveToSeasonListButton){ // move episodes up to adjacent of season folders
		if(showList->currentItem() == NULL) return;
		if(seasonList->currentItem() == NULL) return;
		QList<QListWidgetItem*> names = episodeList->selectedItems();
		QStringList paths; // paths to files to move
		QString showName = showList->currentItem()->text();
		QString seasonName = seasonList->currentItem()->text();
		if(names.length()==0) return; // nothing to move
		for(int x=0; x<names.length(); x++){
			paths.push_back(importLocation+"/"+showName+"/"+seasonName+"/"+names[x]->text());
		}
		moveFilesToFolder(paths,importLocation+"/"+showName);
		QDir().rmdir(importLocation+"/"+showName+"/"+seasonName); // try to delete folder - only succeds if empty

		int currentRow = seasonList->currentRow();
		listShowContents(importLocation+"/"+showName);
		seasonList->setCurrentRow(currentRow%seasonList->count()); // have it list the same season if it still exists
	}
	if(sender == belowListWidgets.moveToShowListButton){ // move seasons up to adjacent of show folders
		if(showList->currentItem() == NULL) return;
		QList<QListWidgetItem*> names = seasonList->selectedItems();
		QStringList paths; // paths to files to move
		QString showName = showList->currentItem()->text();
		if(names.length()==0) return; // nothing to move
		for(int x=0; x<names.length(); x++){
			paths.push_back(importLocation+"/"+showName+"/"+names[x]->text());
		}
		moveFilesToFolder(paths,importLocation);
		QDir().rmdir(importLocation+"/"+showName); // try to delete folder - only succeds if empty

		int currentRow = showList->currentRow();
		listShows(importLocation);
		showList->setCurrentRow(currentRow%showList->count());
	}
	if(sender == belowListWidgets.addToShowButton){ // move seasons up to adjacent of show folders
		if(showList->currentItem() == NULL) return;
		QList<QListWidgetItem*> names = showList->selectedItems();
		QStringList paths; // paths to files to move
		QString newFolder = belowListWidgets.showNameEntry->text();
		if(names.length()==0) return; // nothing to move
		for(int x=0; x<names.length(); x++){
			paths.push_back(importLocation+"/"+names[x]->text());
		}
		moveFilesToFolder(paths,importLocation+"/"+newFolder);

		int currentRow = showList->currentRow();
		listShows(importLocation);
		showList->setCurrentRow(currentRow%showList->count());
	}
}
void MainWindow::moveShowToLibraryPressed(){
	//TODO - make this work across different partitions and mounted volumes
	//rename operations only work on same volumes
	//will have to copy and delete orig to get move behavior
	//maybe subclass QFile and do some read, some write between the two locations so i can emit progress?
	if(showList->currentItem() == NULL) return;
	QString showName = showList->currentItem()->text();
	bool ok =
	renameFileOrFolder(importLocation+"/"+showName ,
					   libraryLocation+"/"+showName
					   );
	listShows(importLocation);

	if(!ok){
		qWarning( ("Unable to move show : "+showName+"\n").toAscii() );
	}
}

void MainWindow::changeEpisodeOrderPressed(){
	QList<QListWidgetItem*> items = episodeList->selectedItems();
	QList<int> itemRows;
	if(items.length()==0) return;
	for(int x=0; x<items.length(); x++){
		itemRows.push_back(episodeList->row(items[x]));
	}
	qSort(itemRows); // make sure it is sorted even though it "should" be

	QObject *sender = QObject::sender();
	if(sender == overrideSettingsWidgets.moveEpisode_Up_button){
		//remove items that can't move up aka like rows 0,1,2,3,4 or just 0
		// so if 0,1,5,6 -> we get 5,6 out
		for(int x=0; itemRows.length()>0 && x==itemRows[0]; x++)
			itemRows.pop_front();
		if(itemRows.length() == 0) return;

		//now go in forward order, swapping rows
		for(int x=0; x<itemRows.length(); x++){
			QListWidgetItem *temp1,*temp2;
			temp1=episodeList->takeItem(itemRows[x]);
			temp2=episodeList->takeItem(itemRows[x]-1);
			episodeList->insertItem(itemRows[x]-1,temp1);
			episodeList->insertItem(itemRows[x]  ,temp2);
			temp1->setSelected(true); // to keep the selected entries selected after moving
		}
	}
	if(sender == overrideSettingsWidgets.moveEpisode_Ignore_button){
		//now go in reverse order, deleting entries
		for(int x=itemRows.length()-1; x>=0; x--){
			delete episodeList->takeItem(itemRows[x]);
		}
	}
	if(sender == overrideSettingsWidgets.moveEpisode_Down_button){
		//remove items that can't move up down past the end of the list
		// so if 7 items total and indicies 0,1,5,6 -> we get 0,1 out
		for(int x=episodeList->count()-1; itemRows.length()>0 && x==itemRows[itemRows.length()-1]; x--)
			itemRows.pop_back();
		if(itemRows.length() == 0) return;

		//now go in reverse order, swapping rows
		for(int x=itemRows.length()-1; x>=0; x--){
			QListWidgetItem *temp1,*temp2;
			temp2=episodeList->takeItem(itemRows[x]+1);
			temp1=episodeList->takeItem(itemRows[x]);
			episodeList->insertItem(itemRows[x]  ,temp2);
			episodeList->insertItem(itemRows[x]+1,temp1);
			temp1->setSelected(true); // to keep the selected entries selected after moving
		}
	}
}

void MainWindow::showSettingsDialog(){
	QDir configLocation(QDir::homePath()+"/.config/glop_conf");
	if(!configLocation.exists())
		configLocation.mkpath(QDir::homePath()+"/.config/glop_conf");
	GlopConfig::Settings s = GlopConfig::ParseFile( (QDir::homePath()+"/.config/glop_conf/simplifiedVideoLibraryRenamer.conf").toStdString() );

	SettingsDialog d(s);
	d.exec();
	s = d.getSettings();
	GlopConfig::SaveToFile( (QDir::homePath()+"/.config/glop_conf/simplifiedVideoLibraryRenamer.conf").toStdString() , s);
	loadConfig();
	listShows(importLocation); // refresh if in a new directory
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
	belowListWidgets.addToShowButton = new QPushButton("Add To Folder");
	belowListWidgets.moveToShowListButton = new QPushButton("Move Left");
	belowListWidgets.moveToSeasonListButton = new QPushButton("Move Left");

	renameSeasonButton = new QPushButton("Rename Season");
	moveShowToLibraryButton = new QPushButton("Move To Library");

	mainLayout->addWidget(belowListWidgets.showNameEntry,3,1);
	mainLayout->addWidget(belowListWidgets.showRenameButton,4,1);
	mainLayout->addWidget(belowListWidgets.seasonNameEntry,3,2);
	mainLayout->addWidget(belowListWidgets.seasonRenameButton,4,2);
	mainLayout->addWidget(belowListWidgets.addToShowButton,5,1);
	mainLayout->addWidget(belowListWidgets.moveToShowListButton,5,2);
	mainLayout->addWidget(belowListWidgets.moveToSeasonListButton,5,3);

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

	connect(belowListWidgets.moveToSeasonListButton,SIGNAL(clicked(bool)),this,SLOT(generalMoveFilesPressed()) );
	connect(belowListWidgets.moveToShowListButton,SIGNAL(clicked(bool)),this,SLOT(generalMoveFilesPressed()) );
	connect(belowListWidgets.addToShowButton,SIGNAL(clicked(bool)),this,SLOT(generalMoveFilesPressed()) );

	connect(settingsButton,SIGNAL(clicked(bool)),this,SLOT(showSettingsDialog()) );
	connect(overrideSettingsWidgets.moveEpisode_Up_button,SIGNAL(clicked(bool)),this,SLOT(changeEpisodeOrderPressed()) );
	connect(overrideSettingsWidgets.moveEpisode_Ignore_button,SIGNAL(clicked(bool)),this,SLOT(changeEpisodeOrderPressed()) );
	connect(overrideSettingsWidgets.moveEpisode_Down_button,SIGNAL(clicked(bool)),this,SLOT(changeEpisodeOrderPressed()) );

	connect(renameSeasonButton,SIGNAL(clicked(bool)),this,SLOT(renameEpisodes()) );
	connect(moveShowToLibraryButton,SIGNAL(clicked(bool)),this,SLOT(moveShowToLibraryPressed()) );
}

void MainWindow::loadConfig(){
	QDir configLocation(QDir::homePath()+"/.config/glop_conf");
	bool ok1,ok2,ok3;
	if(!configLocation.exists())
		configLocation.mkpath(QDir::homePath()+"/.config/glop_conf");
	GlopConfig::Settings s = GlopConfig::ParseFile( (QDir::homePath()+"/.config/glop_conf/simplifiedVideoLibraryRenamer.conf").toStdString() );
	importLocation = QString::fromStdString(s.values["import location"]);
	libraryLocation = QString::fromStdString(s.values["library location"]);
	ok3 = importLocation.length()>0;
	importLocation = QFileInfo(importLocation).absoluteFilePath(); // returns current dir if empty string
	libraryLocation = QFileInfo(libraryLocation).absoluteFilePath();

	QString temp = QString::fromStdString(s.values["season number length"]);
	seasonNumberLength = temp.toInt(&ok1);
	if(!ok1) seasonNumberLength = 5;

	temp = QString::fromStdString(s.values["episode number length"]);
	episodeNumberLength = temp.toInt(&ok2);
	if(!ok2) episodeNumberLength = 5;

	if(!(ok1 && ok2 && ok3)){
		showSettingsDialog();
	}
}
void MainWindow::saveConfig(){
	QDir configLocation(QDir::homePath()+"/.config/glop_conf");
	if(!configLocation.exists())
		configLocation.mkpath(QDir::homePath()+"/.config/glop_conf");
	GlopConfig::Settings s ;

	s.values["import location"] = importLocation.toStdString();
	s.values["library location"] = libraryLocation.toStdString();
	s.values["season number length"] = QString::number(seasonNumberLength).toStdString();
	s.values["episode number length"] = QString::number(episodeNumberLength).toStdString();

	GlopConfig::SaveToFile((QDir::homePath()+"/.config/glop_conf/simplifiedVideoLibraryRenamer.conf").toStdString() , s);
}

bool MainWindow::renameFileOrFolder(QString oldName, QString newName){
	bool ok;
	QFileInfo source(oldName);
	QFileInfo dest(newName);
	if(source.isDir()){
		if(!dest.exists())
			ok = QDir().rename(oldName,newName);
		else{
			ok = true;
			//merge the folders
			QDir sd(oldName); // source directory
			QStringList list = sd.entryList(QDir::NoDotAndDotDot|QDir::Files|QDir::Dirs);
			for(int x=0; x<list.length(); x++){
				ok = ok && sd.rename(oldName+"/"+list[x],newName+"/"+list[x]); // move the files into the new folder
			}
			sd.rmdir(oldName); // safely fails if not empty
		}
	}else{
		if(!dest.exists())
			ok = QFile::rename(oldName,newName);
		//else do nothing
	}
	return ok;
}

bool MainWindow::moveFilesToFolder(QStringList files, QString folder){
	// NOTE: also can pass in folders to the file-list
	QFileInfo destFolder(folder);
	bool ok = true;
	if(!destFolder.exists()){
		QDir().mkpath(folder); // make the folder
	}else if(destFolder.isFile())
		return false; // cant move files into a file :P

	for(int x=0; x<files.length(); x++){
		QFileInfo source(files[x]);
		ok = ok && renameFileOrFolder(files[x],folder+"/"+source.fileName()); // move the file to the new location - does merge folders
	}
	return ok;
}
