#include "settingsdialog.h"

SettingsDialog::SettingsDialog(GlopConfig::Settings s, QWidget *parent) : QDialog(parent) {
	newSettings = s;
	createWidgets();
	setNamingExample();
}
SettingsDialog::~SettingsDialog(){
}

GlopConfig::Settings SettingsDialog::getSettings(){
	return newSettings;
}

void SettingsDialog::reject(){
	QDialog::reject();
}
void SettingsDialog::accept(){
	QDialog::accept();
	newSettings.values["season number length"] = QString::number(seasonNumberLengthEntry->value()).toStdString();
	newSettings.values["episode number length"] = QString::number(episodeNumberLengthEntry->value()).toStdString();

	newSettings.values["import location"] = importLocationEntry->text().toStdString();
	newSettings.values["library location"] = libraryLocationEntry->text().toStdString();
}

void SettingsDialog::openFileDialog(){
	QObject *sender = QObject::sender();
	if(sender == importLocationButton){
		QString temp = QFileDialog::getExistingDirectory(this,"Select Location",importLocationEntry->text());
		if(temp.length()>0) // handle if user hits cancel
			importLocationEntry->setText(temp);
	}
	if(sender == libraryLocationButton){
		QString temp = QFileDialog::getExistingDirectory(this,"Select Location",libraryLocationEntry->text());
		if(temp.length()>0) // handle if user hits cancel
			libraryLocationEntry->setText(temp);
	}
}

void SettingsDialog::setNamingExample(){
	QString showName = "Example Show Title";
	int seasonNumber = 3;
	int episodeNumber = 5;
	QString fileExtension = ".mkv";

	QString seasonString = QString::number(seasonNumber);
	while(seasonString.length()<seasonNumberLengthEntry->value()) seasonString = "0"+seasonString;  // make it long enough as per our settings
	QString episodeString = QString::number(episodeNumber);
	while(episodeString.length()<episodeNumberLengthEntry->value()) episodeString = "0"+episodeString;

	QString newName = showName+".s"+seasonString+"e"+episodeString+fileExtension; // formated as per user settings
	exampleNamedEpisode->setText(newName);
}

void SettingsDialog::createWidgets(){
	seasonNumberLengthLabel = new QLabel("Season Number Length");
	episodeNumberLengthLabel = new QLabel("Episode Number Length");
	seasonNumberLengthEntry = new QSpinBox();
	episodeNumberLengthEntry = new QSpinBox();
	exampleNamedEpisode = new QLabel("example show name here");
	importLocationButton = new QPushButton("Import Location");
	importLocationEntry = new QLineEdit();
	libraryLocationButton = new QPushButton("Library Location (Optional)");
	libraryLocationEntry = new QLineEdit();
	okButton = new QPushButton("OK");
	cancelButton = new QPushButton("Cancel");

	//==================================================
	//all items gridded below here
	layout = new QGridLayout(this);
	layout->addWidget(seasonNumberLengthLabel,1,1);
	layout->addWidget(episodeNumberLengthLabel,1,2);
	layout->addWidget(seasonNumberLengthEntry,2,1);
	layout->addWidget(episodeNumberLengthEntry,2,2);
	layout->addWidget(exampleNamedEpisode,3,1,1,2);

	layout->addWidget(importLocationButton,4,1,1,2);
	layout->addWidget(importLocationEntry,5,1,1,2);
	layout->addWidget(libraryLocationButton,6,1,1,2);
	layout->addWidget(libraryLocationEntry,7,1,1,2);

	layout->addWidget(okButton,100,1);
	layout->addWidget(cancelButton,100,2);

	this->setLayout(layout);

	//==================================================
	//setting in the values here
	int temp;
	temp = QString::fromStdString(newSettings.values["season number length"]).toInt();
	seasonNumberLengthEntry->setValue( temp>0 && temp<15 ? temp : 5 );
	temp = QString::fromStdString(newSettings.values["episode number length"]).toInt();
	episodeNumberLengthEntry->setValue( temp>0 && temp<15 ? temp : 5 );
	seasonNumberLengthEntry->setMaximum(14);
	seasonNumberLengthEntry->setMinimum(1);
	episodeNumberLengthEntry->setMaximum(14);
	episodeNumberLengthEntry->setMinimum(1);

	importLocationEntry->setText( QString::fromStdString(newSettings.values["import location"]) );
	libraryLocationEntry->setText( QString::fromStdString(newSettings.values["library location"]) );

	//==================================================
	//connections here
	connect(okButton,SIGNAL(clicked(bool)),this,SLOT(accept()) );
	connect(cancelButton,SIGNAL(clicked(bool)),this,SLOT(reject()) );

	connect(importLocationButton,SIGNAL(clicked(bool)),this,SLOT(openFileDialog()) );
	connect(libraryLocationButton,SIGNAL(clicked(bool)),this,SLOT(openFileDialog()) );

	connect(seasonNumberLengthEntry,SIGNAL(valueChanged(int)),this,SLOT(setNamingExample()) );
	connect(episodeNumberLengthEntry,SIGNAL(valueChanged(int)),this,SLOT(setNamingExample()) );
}
