#include "renamerwidget.h"
#include "ui_renamerwidget.h"

RenamerWidget::RenamerWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RenamerWidget),
	settingsDialog(new SettingsDialog)
{
	ui->setupUi(this);
	connectSignals();

	listShows();
}

RenamerWidget::~RenamerWidget()
{
	delete ui;
	delete settingsDialog;
}

void RenamerWidget::connectSignals()
{
	connect( ui->bSettingsWindow,SIGNAL(clicked()),	settingsDialog,SLOT(open()) );
	connect( settingsDialog,SIGNAL(accepted()),   this,SLOT(listShows()) );

	connect( ui->listShows,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			 this,SLOT(listShowContents()) );
	connect( ui->listSeasons,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			 this,SLOT(listSeasonContents()) );

	connect( ui->bRenameSingleShow,SIGNAL(clicked()), this,SLOT(renameShow()) );
	connect( ui->bRenameSingleSeason,SIGNAL(clicked()), this,SLOT(renameSeason()) );
	connect( ui->bRenameSeasonEpisodes,SIGNAL(clicked()), this,SLOT(renameEpisodes()) );
	connect( ui->bShowsMoveRight,SIGNAL(clicked()), this,SLOT(moveSelectedFiles()) );
	connect( ui->bSeasonsMoveLeft,SIGNAL(clicked()), this,SLOT(moveSelectedFiles()) );
	connect( ui->bEpisodesMoveLeft,SIGNAL(clicked()), this,SLOT(moveSelectedFiles()) );

	connect( ui->bSeason0,SIGNAL(clicked()), this,SLOT(renameSeasonShortcutButtons()) );
	connect( ui->bSeason1,SIGNAL(clicked()), this,SLOT(renameSeasonShortcutButtons()) );
	connect( ui->bSeason2,SIGNAL(clicked()), this,SLOT(renameSeasonShortcutButtons()) );
	connect( ui->bSeason3,SIGNAL(clicked()), this,SLOT(renameSeasonShortcutButtons()) );
	connect( ui->bSeason4,SIGNAL(clicked()), this,SLOT(renameSeasonShortcutButtons()) );
	connect( ui->bSeason5,SIGNAL(clicked()), this,SLOT(renameSeasonShortcutButtons()) );
	connect( ui->bSeasonExtra,SIGNAL(clicked()), this,SLOT(renameSeasonShortcutButtons()) );
	connect( ui->bSeasonCustom,SIGNAL(clicked()), this,SLOT(renameSeasonShortcutButtons()) );

	connect( ui->bMoveEpisodesUp,SIGNAL(clicked()), this,SLOT(reorderSelectedEpisodes()) );
	connect( ui->bIgnoreEpisodes,SIGNAL(clicked()), this,SLOT(reorderSelectedEpisodes()) );
	connect( ui->bMoveEpisodesDown,SIGNAL(clicked()), this,SLOT(reorderSelectedEpisodes()) );
}


void RenamerWidget::listShows()
{
	QString importLocation = (*settingsDialog)["import location"];
	QDir loc(importLocation);
	if(importLocation.length() <= 0 || !loc.exists())
	{
		settingsDialog->open(); // no settings found
		return; // open() is asynchronous so we are not ready yet
	}

	ui->listShows->clear();
	ui->listSeasons->clear();
	ui->listEpisodes->clear();
	ui->eShowFilename->clear();
	ui->eSeasonFilename->clear();
	QStringList list = loc.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(int x=0; x<list.length(); x++){
		ui->listShows->addItem(list[x]);
	}

	list = loc.entryList(QDir::Files);
	for(int x=0; x<list.length(); x++){
		QListWidgetItem *temp = new QListWidgetItem(list[x]);
		temp->setBackground(Qt::red);
		ui->listShows->addItem(temp);
	}
}
void RenamerWidget::listShowContents()
{
	QListWidgetItem *showName = ui->listShows->currentItem();
	if(showName == nullptr) return;
	ui->eShowFilename->setText(showName->text());

	QString path = (*settingsDialog)["import location"] + "/" + showName->text();
	QDir loc(path);
	if(!loc.exists()) return;

	ui->listSeasons->clear();
	ui->listEpisodes->clear();
	ui->eSeasonFilename->clear();
	QStringList list = loc.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(int x=0; x<list.length(); x++){
		ui->listSeasons->addItem(list[x]);
	}

	list = loc.entryList(QDir::Files);
	for(int x=0; x<list.length(); x++){
		QListWidgetItem *temp = new QListWidgetItem(list[x]);
		temp->setBackground(Qt::red);
		ui->listSeasons->addItem(temp);
	}
}
void RenamerWidget::listSeasonContents()
{
	QListWidgetItem *showName = ui->listShows->currentItem();
	QListWidgetItem *seasonName = ui->listSeasons->currentItem();
	if(showName == nullptr || seasonName == nullptr) return;
	ui->eSeasonFilename->setText(seasonName->text());

	QString path = (*settingsDialog)["import location"] + "/" + showName->text() + "/" + seasonName->text();
	QDir loc(path);
	if(!loc.exists()) return;

	ui->listEpisodes->clear();
	// List all directories at the top with a red background - these will be skipped when renaming
	// note: we are using the background color being red as the flag to skip the item when renaming
	QStringList directoryList = loc.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for(int x=0; x<directoryList.length(); x++){
		QListWidgetItem *temp = new QListWidgetItem(directoryList[x]);
		temp->setBackground(Qt::red);
		ui->listEpisodes->addItem(temp);
	}

	// For convenience, we want to parse the files and group together files that only differ by their extension
	// eg subs files should be rnamed to follow their video file
	// The key is the baseName of thhe file, while all extensions are added to the QStringList
	QMap<QString,QStringList> episodeFilenames;

	{ // parse the filenames and group them together into our above QMap
		bool autoDeleteEnabled = (*settingsDialog)["auto delete enable"] == "true";
		QStringList autoDeleteFilenames = (*settingsDialog)["auto delete filenames"].split(";",Qt::SkipEmptyParts);
		QStringList totalFileList = loc.entryList(QDir::Files);
		for(int x=0; x<totalFileList.length(); x++){
			QFileInfo file(totalFileList[x]);
			if(autoDeleteEnabled && autoDeleteFilenames.contains(totalFileList[x]) )
				// delete known useless files for user convenience
				QFile::remove(path+"/"+totalFileList[x]);
			else
				// put the file in our QStringList to group similar items together
				episodeFilenames[file.completeBaseName()].push_back(file.suffix());
		}
	}

	// Add each of the parsed filename groups to the episode list
	// Each entry in the episode list can be multiline, indicating multiple files for each
	// When renaming, the same basename is given to each file on each newline but the extension is preserved
	for(QString baseName : episodeFilenames.keys()){
		QString listing;
		for(QString ext : episodeFilenames[baseName]){
			listing += baseName+"."+ext+"\n";
		}
		listing.chop(1); // remove the \n on the end
		ui->listEpisodes->addItem(listing);
	}

	// Lets configure the Current Show information on the right side of the window
	QStringList parts = seasonName->text().toLower().split(" "); // split along spaces
	int seasonNumber = 1;
	if( parts.size()==2 && parts[0]=="season" ){ // we only understand the format of "season #"
		seasonNumber = parts[1].toInt(); // assume the second entry is the number
	}
	ui->eParsedShowName->setText(showName->text());
	ui->eParsedSeasonNumber->setValue(seasonNumber);
	ui->eParsedStartingEpisodeNumber->setValue(1);
	ui->eNumberEpisodesPerFile->setValue(1);
}

void RenamerWidget::renameShow()
{
	QListWidgetItem *showName = ui->listShows->currentItem();
	if(showName == nullptr) return;

	QString oldPath = (*settingsDialog)["import location"] + "/" + showName->text();
	QString newPath = (*settingsDialog)["import location"] + "/" + ui->eShowFilename->text();

	renameFileOrFolder(oldPath,newPath);

	listShows();
}
void RenamerWidget::renameSeason()
{
	QListWidgetItem *showName = ui->listShows->currentItem();
	QListWidgetItem *seasonName = ui->listSeasons->currentItem();
	if(showName == nullptr || seasonName == nullptr) return;

	QString oldPath = (*settingsDialog)["import location"] + "/" + showName->text() + "/" + seasonName->text();
	QString newPath = (*settingsDialog)["import location"] + "/" + showName->text() + "/" + ui->eSeasonFilename->text();

	renameFileOrFolder(oldPath,newPath);

	listShowContents();
}
void RenamerWidget::renameSeasonShortcutButtons()
{
	QListWidgetItem *showName = ui->listShows->currentItem();
	if(showName==nullptr) return;
	QString root = (*settingsDialog)["import location"]+"/"+showName->text();
	QList<QListWidgetItem*> oldNames = ui->listSeasons->selectedItems();
	QString newName;
	if(oldNames.length() == 0) return;

	QObject* sender = this->sender();
	if(sender == ui->bSeason0){
		newName = "Season 0";
	}else if (sender == ui->bSeason1) {
		newName = "Season 1";
	}else if (sender == ui->bSeason2) {
		newName = "Season 2";
	}else if (sender == ui->bSeason3) {
		newName = "Season 3";
	}else if (sender == ui->bSeason4) {
		newName = "Season 4";
	}else if (sender == ui->bSeason5) {
		newName = "Season 5";
	}else if (sender == ui->bSeasonExtra) {
		newName = "Extra";
	}else if (sender == ui->bSeasonCustom) {
		newName = "Season "+ui->eSeasonCustomNumber->text();
	}

	if( (!ui->cSeasonMoveOnly->isChecked()) && oldNames.length() == 1 && QDir(root+"/"+oldNames[0]->text()).exists()){
		// Special case of having a single Directory selected
		renameFileOrFolder(root+"/"+oldNames[0]->text() , root+"/"+newName);
	}else{
		QStringList oldPaths;
		for(auto name : oldNames){
			oldPaths.push_back(root+"/"+name->text());
		}
		moveFilesToFolder(oldPaths,root+"/"+newName);
	}

	listShowContents();
}

void RenamerWidget::renameEpisodes()
{
	QListWidgetItem *showItem = ui->listShows->currentItem();
	QListWidgetItem *seasonItem = ui->listSeasons->currentItem();
	if(showItem == nullptr || seasonItem == nullptr) return;
	QString basePath = (*settingsDialog)["import location"] + "/" + showItem->text() + "/" + seasonItem->text() + "/";

	QString showName = ui->eParsedShowName->text();
	int seasonNumber = ui->eParsedSeasonNumber->value();
	int episodeNumber = ui->eParsedStartingEpisodeNumber->value();
	int episodesPerFile = ui->eNumberEpisodesPerFile->value();

	for(int row=0 ; row<ui->listEpisodes->count() ; row++){
		auto item = ui->listEpisodes->item(row);
		if(item->background() == Qt::red) continue;

		QString newBasename = showName;
		newBasename += ".s"+QString::number(seasonNumber).rightJustified((*settingsDialog)["season number length"].toInt(),'0');
		for(int ep=0 ; ep<episodesPerFile ; ep++,episodeNumber++){
			newBasename += "e"+QString::number(episodeNumber).rightJustified((*settingsDialog)["episode number length"].toInt(),'0');
		}
		newBasename += ".";

		for(QString oldFileName : item->text().split("\n",Qt::SkipEmptyParts)){
			QString ext = QFileInfo(basePath+oldFileName).suffix();
			renameFileOrFolder(basePath+oldFileName, basePath+newBasename+ext);
		}
	}

	listSeasonContents();
}

void RenamerWidget::moveSelectedFiles()
{
	QString root = (*settingsDialog)["import location"];
	QStringList oldNames;
	QString oldRoot;
	QString newRoot;
	void(RenamerWidget::*rescanFilesFunc)() = &RenamerWidget::listShows; // magic function pointer to non-static member functions

	QObject* sender = this->sender();
	if(sender == ui->bShowsMoveRight){
		for(auto name : ui->listShows->selectedItems())
			oldNames.push_back(name->text());
		oldRoot = root;
		newRoot = root+"/"+ui->eShowFilename->text();
	}else if (sender == ui->bSeasonsMoveLeft) {
		for(auto name: ui->listSeasons->selectedItems())
			oldNames.push_back(name->text());
		oldRoot = root+"/"+ui->listShows->currentItem()->text();
		newRoot = root;
	}else if (sender == ui->bEpisodesMoveLeft) {
		// we have multiple files per entry for episode listings (combines .mkv and .sub files into one item
		for(auto name_pre : ui->listEpisodes->selectedItems() )
			for(auto name : name_pre->text().split("\n",Qt::SkipEmptyParts))
				oldNames.append(name);
		oldRoot = root+"/"+ui->listShows->currentItem()->text()+"/"+ui->listSeasons->currentItem()->text();
		newRoot = root+"/"+ui->listShows->currentItem()->text();
		rescanFilesFunc = &RenamerWidget::listShowContents;
	}
	if(oldNames.length() == 0) return;

	QStringList oldPaths;
	for(auto name : oldNames){
		oldPaths.push_back(oldRoot+"/"+name);
	}
	moveFilesToFolder(oldPaths,newRoot);
	QDir().rmdir(oldRoot); // try to delete folder - only succeds if empty

	(this->*rescanFilesFunc)();
}

void RenamerWidget::reorderSelectedEpisodes()
{
	QList<QListWidgetItem*> selected = ui->listEpisodes->selectedItems();
	if(selected.length() == 0) return;
	QList<int> rows;
	for(auto item : selected)
		if(item->background() != Qt::red) // ignore directories
			rows.push_back(ui->listEpisodes->row(item));
	if(rows.length() == 0) return; // no files selected

	QObject *sender = QObject::sender();
	sender == ui->bMoveEpisodesUp ?
		std::sort(rows.begin(),rows.end()):
		std::sort(rows.rbegin(),rows.rend()); // sorts it backwards if we want to move downwards or ignore episodes

	if(sender == ui->bIgnoreEpisodes){
		for(int row : rows){
			delete ui->listEpisodes->takeItem(row);
		}
		return;
	}

	// General steps
	// 1) remove the first rows if they are already at the top/bottom of the list
	//    as they cannot move any further. This means 0,1,2,6,7 will end up with
	//    only 6,7 as canidates to move if you are moving up
	// 2) swap one row at a time with the position it is supposed to be in
	// 3) make sure the newly placed item is selected to allow repeat moves


	if(sender == ui->bMoveEpisodesUp){
		//Find what row is the first row that is a file and not a directory
		int topFileRow = 0;
		while( topFileRow<ui->listEpisodes->count() && ui->listEpisodes->item(topFileRow)->background() == Qt::red )
				topFileRow += 1;
		//remove selections already at the top that we cannot move any further
		while(rows.size() > 0 && rows[0] == topFileRow){
				topFileRow += 1;
				rows.pop_front();
		}
		if(rows.size() == 0) return;
		//now we swap the rows
		for(int row : rows){
			QListWidgetItem *item = ui->listEpisodes->takeItem(row);
			ui->listEpisodes->insertItem(row-1,item);
			item->setSelected(true);
		}
	}else if(sender == ui->bMoveEpisodesDown){
		int bottomFileRow = ui->listEpisodes->count()-1;
		//remove selections already at the bottom and can't move further down
		while(rows.size() > 0 && rows[0] == bottomFileRow){
			rows.pop_front();
			bottomFileRow -= 1;
		}
		if(rows.size() == 0) return;
		//now we swap the rows
		for(int row : rows){
			QListWidgetItem *item = ui->listEpisodes->takeItem(row);
			ui->listEpisodes->insertItem(row+1,item);
			item->setSelected(true);
		}
	}
}

bool RenamerWidget::renameFileOrFolder(QString oldName, QString newName){
	bool ok=true;
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

bool RenamerWidget::moveFilesToFolder(QStringList files, QString folder){
	// NOTE: also can pass in folders to the file-list
	QFileInfo destFolder(folder);
	bool ok = true;
	if(!destFolder.exists()){
		QDir().mkpath(folder); // make the folder
	}else if(destFolder.isFile())
		return false; // cant move files into a file :P

	for(int x=0; x<files.length(); x++){
		QFileInfo source(files[x]);
		// move the file to the new location - does merge folders
		ok = ok && renameFileOrFolder(files[x],folder+"/"+source.fileName());
	}
	return ok;
}
