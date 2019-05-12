#include "sw.h"
#include "ui_sw.h"
#include <QSettings>
#include <QFileDialog>
#include <QDebug>

sw::sw(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::sw)
{
	ui->setupUi(this);
	ui->librarylabel->setText(loadsettings("library","None Set"));

	ui->sourcelabel->setText(loadsettings("source","None Set"));
	int state = loadsettings("auto","0").toInt();
	if(state)
		ui->autoindex->setChecked(true);
}

sw::~sw()
{
	delete ui;
}

void sw::closeEvent (QCloseEvent *event){
	this->hide();
}

void sw::savesettings(QString key, QString value) {
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");

	QSettings settings("settings.ini",QSettings::IniFormat);
	 settings.setValue(key, value);
}

void sw::savesettings(QString key, int value) {
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");

	QSettings settings("settings.ini",QSettings::IniFormat);
	settings.setValue(key, value);
}

QString sw::loadsettings(QString key, QString defaultval) {
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");

	QSettings settings("settings.ini",QSettings::IniFormat);
	return settings.value(key, defaultval).toString();
}

void sw::on_librarybutton_clicked()
{
	QString directory = QFileDialog::getExistingDirectory(this, ("Select Library Folder"),QDir::homePath());
	ui->librarylabel->setText(directory);
	savesettings("library",directory);
}

void sw::on_sourcebutton_clicked()
{
	QString directory = QFileDialog::getExistingDirectory(this, ("Select Source Folder"), QDir::homePath());
	ui->sourcelabel->setText(directory);
	savesettings("source",directory);
}



void sw::on_autoindex_stateChanged(int arg1)
{
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");

	QSettings settings("settings.ini",QSettings::IniFormat);
	settings.setValue("auto", arg1);
}
