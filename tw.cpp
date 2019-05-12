#include "tw.h"
#include "ui_tw.h"
#include "wb.h"
#include <QDirIterator>
#include <QSettings>
#include <QDebug>
#include <QJsonObject>
#include <QtConcurrent/QtConcurrent>
#include <classes/data/databasehandler.h>
#include <QFuture>
#include <QFutureWatcher>
#include <QGridLayout>
#include <classes/process/linked_mem_buffer.h>

/**
 * The Tools Window class should provided links to associated standalone programs that provide utility functions.
 * These are launched separately to the main window so that they can run with less overhead or risk of thread errors/assertion fails
 * and alo so that they can be added/changed/removed without needing to update the entire program
 * Some of these may be gui and some may be commandline
 * strcture: whitebox/tools/generictool.exe
 * whitebox/tools/manifest.wbm
 * manifest names and descriptions of each tool which is used to load them into the program so they can be run
 */

/**
 * todo: add merge shows (eg, move all 'arrow' shows into 'Arrow', 'mythbusters' into 'Mythbusters', etc)
 * todo: rename show
 * todo: find potential duplicates ('arrow' vs 'Arrow' etc.)
 *
 *
 */



tw::tw(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::tw)
{
	ui->setupUi(this);
	library = loadsettings("library","");
	source = loadsettings("source", "");
	connect(&build_index_watcher, SIGNAL(finished()), this, SLOT(build_index_finished()));


	connect(&show_rename_watcher, SIGNAL(finished()), this, SLOT(show_rename_finished()));
}

tw::~tw()
{
	this->hide();
}

void tw::closeEvent (QCloseEvent *event){
	this->hide();
}


void tw::savesettings(QString key, QString value) {
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");

	QSettings settings("settings.ini",QSettings::IniFormat);
	settings.setValue(key, value);
}

void tw::savesettings(QString key, int value) {
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");
	QSettings settings("settings.ini",QSettings::IniFormat);
	settings.setValue(key, value);
}

QString tw::loadsettings(QString key, QString defaultval) {
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");

	QSettings settings("settings.ini",QSettings::IniFormat);
	return settings.value(key, defaultval).toString();
}



/** SAVE FILE INFO TO DATABSE **/
void tw::on_build_index_clicked()
{
	ui->build_index->setDisabled(true);
	build_index();
}

void tw::build_index(){
	build_index_future = QtConcurrent::run(async_index,library,source);
	build_index_watcher.setFuture(build_index_future);
}

void tw::build_index_finished(){
	ui->build_index->setDisabled(false);

}

void tw::index_public_button(){
	on_build_index_clicked();
}


bool tw::async_index(QString location,QString location2)
{
	/**
	 * @brief prototype : build library db from found files
	 * @return void, resulting data accessed through data.db
	 * this should probably be a tools/tool.exe to run async to main program
	 */

	QString runlocation = location;
	start:


	//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	//Tests for raceconditions when there are no files attached to sort through - prevents instant completion, prevent races etc


	if (runlocation != "") {


		QDirIterator iterator(runlocation, QDirIterator::Subdirectories);

		while (iterator.hasNext()) {
			QString fname = iterator.next();
			QFileInfo fileinfo(fname);

			if (fileinfo.suffix() == "" || fileinfo.suffix() == "nfo")
				continue;

			QJsonObject object = QJsonObject();

			QString showname = "";
			int season;
			int episode;

			if (wb::extract_episode_details(fileinfo.absoluteFilePath(), showname, season, episode)) {

				object["path"] = fileinfo.absoluteFilePath();
				object["lastwatch"] = NULL;
				object["playcount"] = 0;
				object["showname"] = showname;
				object["season"] = season;
				object["episode"] = episode;

				databasehandler::insertdata("shows", object);

			}
		}
	}

	if(runlocation == location2)
		return true;

	runlocation = location2;
	goto start;
}


/** Rename Show **/

void tw::on_show_rename_clicked()
{
	ui->show_rename->setDisabled(true);
	show_rename();


}


bool tw::async_rename(QString from, QString to){

	QString query = "Update shows set showname = \'"+to+"\' where showname = \'"+from+"\' ";

	return true;
}

void tw::show_rename(){

	QLabel* fromlabel = new QLabel;
	QLabel* tolabel = new QLabel;
	QLabel* customtolabel = new QLabel;


	fromlabel->setText("Show to rename");
	tolabel->setText("Rename to...");
	customtolabel->setText("Or choose a new name");


	newname = new QLineEdit;
	showlistfrom = new QListWidget;
	showlistto = new QListWidget;

	QPushButton* startrename = new QPushButton;
	startrename->setText("Go");

	QString query = "select distinct `showname` from `shows` ORDER BY `showname` COLLATE NOCASE";

	QSqlQuery result = databasehandler::execquery(query);

	do {
		showlistfrom->addItem(result.value("showname").toString());

		showlistto->addItem(result.value("showname").toString());
	} while (result.next());


	ui->tool_window_controls->layout()->addWidget(fromlabel);
	ui->tool_window_controls->layout()->addWidget(showlistfrom);

	ui->tool_window_controls->layout()->addWidget(tolabel);
	ui->tool_window_controls->layout()->addWidget(showlistto);


	ui->tool_window_controls->layout()->addWidget(customtolabel);
	ui->tool_window_controls->layout()->addWidget(newname);
	ui->tool_window_controls->layout()->addWidget(startrename);


	connect(startrename, SIGNAL(clicked()), this, SLOT(show_rename_run()));



}

void tw::show_rename_finished(){
	ui->show_rename->setDisabled(false);
	qDebug()  << " showrename finished " << endl;
}


void tw::show_rename_run(){

	int index = showlistfrom->currentRow();
	if(index != -1)
		qDebug() << showlistfrom->currentItem()->text() << endl;
	else {
		qDebug() << "no from show selected" << endl;
	}


	int index2 = showlistto->currentRow();
	if(index2 != -1)
		qDebug() << showlistto->currentItem()->text() << endl;
	else {
		qDebug() << "no to show selected" << endl;
	}

	qDebug() << newname->text() << endl;
}

void tw::on_dbg_clicked()
{
	//
}
