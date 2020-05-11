#include "settingsdialog.h"
#include "ui_settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Settings)
{
	ui->setupUi(this);

	QDir configLocation(QDir::homePath()+"/.config/glop_conf");
	if(!configLocation.exists())
		configLocation.mkpath(configLocation.path());
	settings = GlopConfig::ParseFile( configLocation.filePath("simplifiedVideoLibraryRenamer.conf").toStdString() );

	connect( ui->eSeasonLength,SIGNAL(valueChanged(int)), this,SLOT(updateExampleRename()) );
	connect( ui->eEpisodeLength,SIGNAL(valueChanged(int)), this,SLOT(updateExampleRename()) );

	connect( ui->bBrowseImport,SIGNAL(clicked()), this,SLOT(openFileDialog()) );
	connect( ui->bBrowseLibrary,SIGNAL(clicked()), this,SLOT(openFileDialog()) );

	connect( ui->bConfirm,SIGNAL(clicked()), this,SLOT(accept()) );
	connect( ui->bCancel,SIGNAL(clicked()), this,SLOT(reject()) );
}

QString SettingsDialog::operator[](QString key)
{
	return QString::fromStdString(
			settings.values[key.toStdString()]
			);
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}


void SettingsDialog::open()
{
	QDialog::open();
	ui->eSeasonLength->setValue(
				QString::fromStdString(settings.values["season number length"]).toInt()
				);
	ui->eEpisodeLength->setValue(
				QString::fromStdString(settings.values["episode number length"]).toInt()
				);
	ui->eImportLocation->setText(
				QString::fromStdString(settings.values["import location"])
				);
	ui->eLibraryLocation->setText(
				QString::fromStdString(settings.values["library location"])
				);

	updateExampleRename();
}

void SettingsDialog::accept()
{
	// lets save the settings to the config file
	settings.values["season number length"] =
			ui->eSeasonLength->text().toStdString();
	settings.values["episode number length"] =
			ui->eEpisodeLength->text().toStdString();
	settings.values["import location"] =
			ui->eImportLocation->text().toStdString();
	settings.values["library location"] =
			ui->eLibraryLocation->text().toStdString();

	QDir configLocation(QDir::homePath()+"/.config/glop_conf/simplifiedVideoLibraryRenamer.conf");
	GlopConfig::SaveToFile(configLocation.path().toStdString(),settings);

	QDialog::accept();
}

void SettingsDialog::updateExampleRename()
{
	QString season = QString::number(3).rightJustified(ui->eSeasonLength->value(),'0');
	QString episode = QString::number(5).rightJustified(ui->eEpisodeLength->value(),'0');

	// Example Show Title.s03e005.mkv
	QString text("Example Show Title.s#e%.mkv");
	text.replace('#',season);
	text.replace('%',episode);

	ui->lExampleRename->setText(text);
}

void SettingsDialog::openFileDialog(){
	QObject *sender = QObject::sender();
	if(sender == ui->bBrowseImport){
		QString temp = QFileDialog::getExistingDirectory(this,"Select Location",ui->eImportLocation->text());
		if(temp.length()>0) // handle if user hits cancel
			ui->eImportLocation->setText(temp);
	}
	if(sender == ui->bBrowseLibrary){
		QString temp = QFileDialog::getExistingDirectory(this,"Select Location",ui->eLibraryLocation->text());
		if(temp.length()>0) // handle if user hits cancel
			ui->eLibraryLocation->setText(temp);
	}
}
