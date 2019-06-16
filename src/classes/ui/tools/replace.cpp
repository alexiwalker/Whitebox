#include "replace.h"
#include <QtConcurrent/QtConcurrent>
#include <chrono>
#include <thread>
#include <classes/process/util.h>
#include <QFile>
#include <classes/data/databasehandler.h>
#include <QSqlQuery>
#include <QListWidget>

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

	connect(execute_button, SIGNAL(clicked()), this, SLOT(exec()));


	connect(showlist, SIGNAL(currentRowChanged(int)), this, SLOT(loadseasons(int)));
	connect(seasonlist, SIGNAL(currentRowChanged(int)), this, SLOT(loadepisodes(int)));
	connect(episodelist, SIGNAL(currentRowChanged(int)), this, SLOT(setepisode(int)));

	main_layout->addWidget(showlist);
	main_layout->addWidget(seasonlist);
	main_layout->addWidget(episodelist);

	selectedepisode = new QLabel;
	selectedepisode->setText("Selected show: None");

	main_layout->addWidget(selectedepisode);
	main_layout->addWidget(execute_button);

}

void replace::exec(){
	exec_future = QtConcurrent::run(async_exec);
	exec_watcher.setFuture(exec_future);
	execute_button->setDisabled(true);
}

void replace::async_exec(){
	util::sleepSeconds (10);
}

void replace::exec_finished(){
	execute_button->setEnabled(true);
	emit execute_finished();
}

void replace::setepisode(int row){

	epnum = episodelist->item(row)->text().replace("Episode ","").toInt();
	qDebug() << seasnum << endl;

	QString query = QString("select path from shows where showname = '%1' and episode=%2 and season=%3")
			.arg(showname)
			.arg(QString::number(epnum))
			.arg(QString::number(seasnum));

	QSqlQuery result = databasehandler::execquery(query);
	QString path = "None";
	if(result.isValid())
		path = result.value("path").toString();

	selectedepisode->setText("Selected show: " + path);
}

void replace::loadseasons(int row){

	if (row < 0)
		return;

	seasonlist->clear();
	episodelist->clear();
	selectedepisode->setText("Selected show: None aaaaaaaaaa");

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


bool replace::update_database_path(QString oldpath, QString newpath){

	QString query =QString("update shows set path = '%1' where path = '%2'").arg(oldpath,newpath);
	QSqlQuery result  = databasehandler::execquery(query);

	return (result.lastError().type() == QSqlError::NoError);

}

bool replace::move_new_file(QString frompath, QString topath){
	return QFile(frompath).rename(topath);
}

