#include "settingswindow.h"
#include "ui_settingswindow.h"
#include <QSettings>
#include <QFileDialog>
#include <QDebug>
#include <classes/process/settings.h>

settingsWindow::settingsWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::sw)
{
	ui->setupUi(this);
	ui->librarylabel->setText(settings::load_setting("library","None Set"));

	ui->sourcelabel->setText(settings::load_setting("source","None Set"));
	int state = settings::load_setting("auto","0").toInt();
	if(state)
		ui->autoindex->setChecked(true);
}

settingsWindow::~settingsWindow()
{
	delete ui;
}

void settingsWindow::closeEvent (QCloseEvent *event){
	this->hide();
}


void settingsWindow::on_librarybutton_clicked()
{
	QString directory = QFileDialog::getExistingDirectory(this, ("Select Library Folder"),QDir::homePath());
	ui->librarylabel->setText(directory);
	settings::save_setting("library",directory);
}

void settingsWindow::on_sourcebutton_clicked()
{
	QString directory = QFileDialog::getExistingDirectory(this, ("Select Source Folder"), QDir::homePath());
	ui->sourcelabel->setText(directory);
	settings::save_setting("source",directory);
}



void settingsWindow::on_autoindex_stateChanged(int arg1)
{
	settings::save_setting("auto", arg1);
}
