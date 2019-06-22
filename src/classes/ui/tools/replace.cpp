#include "replace.h"
#include <QtConcurrent/QtConcurrent>
#include <chrono>
#include <thread>
#include <classes/process/util.h>
#include <QFile>
#include <classes/data/databasehandler.h>
#include <QSqlQuery>
#include <QFileDialog>
#include <QListWidget>
#include <QMessageBox>


QString replace::replace_connection = "DATABSE_REPLACE_CONNECTION";

replace::replace()
{
	create_ui();
}

replace::~replace(){
	delete execute_button;
	delete main_layout;
}

void replace::create_ui(){
	selectedfilestring = "";
	selectedfilestring2 = "";
	connect(&exec_watcher, SIGNAL(finished()), this, SLOT(exec_finished()));

	main_layout = new QGridLayout;

	execute_button = new QPushButton;

	execute_button->setText("Execute replace");

	this->setLayout(main_layout);

	QString query = "select distinct showname from shows";
	QSqlQuery result = databasehandler::execquery(query);

	showlist = new QListWidget;
	seasonlist = new QListWidget;
	episodelist = new QListWidget;
	if (result.isValid()){
		do {
			showlist->addItem(result.value("showname").toString());


		} while(result.next());

	}

	selectedfile = new QLabel;
	openfilechoose = new QPushButton;

	openfilechoose->setText("Choose replacement File");
	selectedfile->setText("No File selected");

	main_layout->addWidget(openfilechoose,0,0);
	main_layout->addWidget(selectedfile,0,1);

	connect(openfilechoose, SIGNAL(clicked()), this, SLOT(choosefile()));

	connect(execute_button, SIGNAL(clicked()), this, SLOT(exec()));


	connect(showlist, SIGNAL(currentRowChanged(int)), this, SLOT(loadseasons(int)));
	connect(seasonlist, SIGNAL(currentRowChanged(int)), this, SLOT(loadepisodes(int)));
	connect(episodelist, SIGNAL(currentRowChanged(int)), this, SLOT(setepisode(int)));


	main_layout->addWidget(showlist,main_layout->rowCount(),0,1,0);
	main_layout->addWidget(seasonlist,main_layout->rowCount(),0,1,0);
	main_layout->addWidget(episodelist,main_layout->rowCount(),0,1,0);

	selectedepisode = new QLabel;
	selectedepisode->setText("Selected show: None");


	main_layout->addWidget(selectedepisode,main_layout->rowCount(),0,1,0);
	main_layout->addWidget(execute_button,main_layout->rowCount(),0,1,0);

}

void replace::exec(){

	util::notification("hi");


	if (selectedfilestring == "" || selectedfilestring2 == ""){
		QString message = "Please select an episode to replace and a file to replace it with";

		util::alert(message);
		return;
	}

	exec_future = QtConcurrent::run(async_exec, selectedfilestring, selectedfilestring2);
	exec_watcher.setFuture(exec_future);
	execute_button->setDisabled(true);
}

util::success_message* replace::async_exec(QString newpath,QString oldpath ){

	bool test = false;
	QString message = "could not delete "+ oldpath;
	if(delete_old_show(oldpath)){
		message = "could not update database";
		if(update_database_path(oldpath,newpath)){
			message = "could not move new file";
			test = move_new_file(newpath,oldpath);
			if(test)
				message = "ok";
		}
	}

	util::success_message* result = new util::success_message(test,message);
	return result;
}

void replace::exec_finished(){
	util::success_message* res = exec_future.result();

	if(res->result)
		util::notification("File successfully replaced");
	else
		util::alert("File replacement failed: " + res->message);

	execute_button->setEnabled(true);
	reset_ui();
	delete res;
	emit execute_finished();
}

void replace::choosefile(){
	selectedfilestring = QFileDialog::getOpenFileName(this, ("Select Replacement File"), QDir::homePath(),"Videos (*.mkv *.mp4 *.mov *.avi)");
	selectedfile->setText("File selected: "+ selectedfilestring);

}

void replace::setepisode(int row){

	if(row < 0)
		return;

	epnum = episodelist->item(row)->text().replace("Episode ","").toInt();

	QString query = QString("select path from shows where showname = '%1' and episode=%2 and season=%3")
			.arg(showname)
			.arg(QString::number(epnum))
			.arg(QString::number(seasnum));

	QSqlQuery result = databasehandler::execquery(query);
	QString path = "None";
	if(result.isValid())
		path = result.value("path").toString();
	selectedfilestring2 = path;
	selectedepisode->setText("Selected show: " + path);
}

void replace::loadseasons(int row){
	episodelist->clear();
	seasonlist->clear();

	if (row < 0)
		return;

	selectedepisode->setText("Selected show: None");
	QString show = showlist->item(row)->text();
	showname = show;

	QString query = QString("select distinct season from shows where showname = '%1'").arg(show);

	QSqlQuery result = databasehandler::execquery(query);

	if(result.isValid()){
		do {
			seasonlist->addItem("Season " + result.value("season").toString());
		} while (result.next());
	}


}


void replace::loadepisodes(int row){
	if (row < 0)
		return;

	episodelist->clear();

	QString show = showlist->selectedItems().first()->text();

	QString season = seasonlist->item(row)->text().replace("Season ","");
	seasnum = season.toInt();
	QString query = QString("select distinct episode from shows where showname = '%1' and season = %2").arg(show).arg(season);

	QSqlQuery result = databasehandler::execquery(query);

	if(result.isValid()){
		do {
			episodelist->addItem("Episode " + result.value("episode").toString());
		} while (result.next());
	}


}

bool replace::delete_old_show(QString path){
	return QFile(path).remove();
}

void replace::reset_ui(){
	selectedepisode->setText("Selected show: None");
	selectedfile->setText("No File selected");
	selectedfilestring="";
	selectedfilestring2="";
	episodelist->clear();
	seasonlist->clear();
	showlist->clear();
}

bool replace::update_database_path(QString oldpath, QString newpath){

	QString query =QString("update shows set path = '%1' where path = '%2'").arg(oldpath,newpath);
	QSqlQuery result  = databasehandler::execquery(query);

	return (result.lastError().type() == QSqlError::NoError);

}

bool replace::move_new_file(QString frompath, QString topath){
	return QFile(frompath).rename(topath);
}

