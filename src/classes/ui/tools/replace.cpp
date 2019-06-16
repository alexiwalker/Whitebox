#include "replace.h"
#include <QtConcurrent/QtConcurrent>
#include <chrono>
#include <thread>
#include <classes/process/util.h>
#include <QFile>
#include <classes/data/databasehandler.h>
#include <QSqlQuery>

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

	main_layout->addWidget(execute_button,1,0);

	connect(execute_button, SIGNAL(clicked()), this, SLOT(exec()));

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






















