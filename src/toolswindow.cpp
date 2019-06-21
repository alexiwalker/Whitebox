#include "toolswindow.h"
#include "ui_toolswindow.h"
#include "whitebox.h"
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
#include <classes/ui/tools/reconcile.h>
#include <classes/ui/tools/replace.h>
#include <classes/process/settings.h>
#include <classes/process/util.h>

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
 *
 * todo: add merge shows (eg, move all 'arrow' shows into 'Arrow', 'mythbusters' into 'Mythbusters', etc)
 * todo: rename show
 * todo: find potential duplicates ('arrow' vs 'Arrow' etc.)
 *
 */

QString toolsWindow::LIBRARY_INDEX_CONNECTION = "TOOLS_WINDOW_DATABSE_CONNECTION";

toolsWindow::toolsWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::tw)
{
	ui->setupUi(this);
	library = settings::load_setting("library","");
	source = settings::load_setting("source", "");
	connect(&build_index_watcher, SIGNAL(finished()), this, SLOT(build_index_finished()));
	connect(&show_rename_watcher, SIGNAL(finished()), this, SLOT(show_rename_finished()));
}

toolsWindow::~toolsWindow()
{
	this->hide();
}

void toolsWindow::closeEvent (QCloseEvent *event){
	/**
	 * Warning: 'event' unreferenced formal parameter:
	 * As it is overriding an existing function, the blueprint must be the same
	 * However, this parameter is not required for use at this time, but is available incase it is later
	**/
	this->hide();
}


/** SAVE FILE INFO TO DATABSE **/
void toolsWindow::on_build_index_clicked()
{
	ui->build_index->setDisabled(true);
	build_index();
}

void toolsWindow::build_index(){
	build_index_future = QtConcurrent::run(async_index,library,source);
	build_index_watcher.setFuture(build_index_future);
}

void toolsWindow::build_index_finished(){
	ui->build_index->setDisabled(false);

}

void toolsWindow::index_public_button(){
	on_build_index_clicked();
}


bool toolsWindow::async_index(QString location,QString location2)
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

			if (whiteBox::extract_episode_details(fileinfo.absoluteFilePath(), showname, season, episode)) {

				QSqlQuery renames = databasehandler::execquery ("select * from `auto_rename` where `from` = '"+showname+"'", LIBRARY_INDEX_CONNECTION);
				if(renames.isValid ()){
					showname = renames.value("to").toString ();
				}

				object["path"] = fileinfo.absoluteFilePath();
				object["lastwatch"] = NULL;
				object["playcount"] = 0;
				object["showname"] = showname;
				object["season"] = season;
				object["episode"] = episode;

				databasehandler::insertdata("shows", object, LIBRARY_INDEX_CONNECTION);

			}
		}
	}

	if(runlocation == location2)
		return true;

	runlocation = location2;
	goto start;
}


/** Rename Show **/

void toolsWindow::on_show_rename_clicked()
{
	ui->show_rename->setDisabled(true);
	show_rename();
}


bool toolsWindow::async_rename(QString from, QString to){

	QString query = "Update shows set showname = \'"+to+"\' where showname = \'"+from+"\' ";

	return true;
}

void toolsWindow::show_rename(){

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

void toolsWindow::show_rename_finished(){
	ui->show_rename->setDisabled(false);
	qDebug()  << " showrename finished " << endl;
}


void toolsWindow::show_rename_run(){

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

	//qDebug() << newname->text() << endl;
}

void toolsWindow::on_dbg_clicked()
{
	util::clearLayout(ui->tool_window_controls->layout());
	reconcile* reconcilewidget = new reconcile;

	ui->tool_window_controls->layout()->addWidget(reconcilewidget);

}

void toolsWindow::on_show_images_clicked()
{
	QString query = "select distinct showname from shows";
	QSqlQuery result = databasehandler::execquery(query);
	QJsonObject jobject;
	if(result.isValid()){
		do {
			jobject[result.value("showname").toString()]="";
		} while (result.next());


	QJsonDocument doc(jobject);
	QString jstring(doc.toJson());

	qDebug() << jstring << endl;

	}

}

void toolsWindow::on_replace_clicked()
{
	util::clearLayout(ui->tool_window_controls->layout());
	replace* replacewidget = new replace;
	ui->tool_window_controls->layout()->addWidget(replacewidget);
}
