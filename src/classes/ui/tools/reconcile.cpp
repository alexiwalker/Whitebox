#include <QSqlQuery>
#include <QFile>
#include <QDebug>
#include <QGridLayout>
#include <QFuture>
#include <QLabel>
#include <vector>
#include <QDir>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <classes/data/databasehandler.h>
#include <classes/process/settings.h>
#include <classes/process/util.h>

#include "reconcile.h"
/*
 * Tooling to move all shows in the db to the library location
 * changes any showname that can be evaluated to showname_s08e02 form to a standardised version
 */

QString reconcile::reconcile_connection = "reconcile_shows_connection";

reconcile::reconcile()
{
	create_ui();
}



QString reconcile::create_name(QString file, QString &basepath){
	QString name = "";
	QString querystring = "Select * from shows where path = '"+file+"'";
	QSqlQuery result = databasehandler::execquery(querystring,reconcile_connection);

	if (result.isValid()){
		QString showname = result.value("showname").toString();

		QString season = result.value("season").toString();

		QString episode = result.value("episode").toString();

		querystring = "select * from auto_rename where from = "+showname;

		result = databasehandler::execquery(querystring,reconcile_connection);

		if (result.isValid())
			showname = result.value("to").toString();

		QString base = settings::load_setting("library", "");

		QFileInfo f(file);
		QString extension = f.suffix();

		//moves it to the correct folder
		QString folderlocation = base + "/" + showname + "/season " + season;
		basepath = folderlocation;
		//sets name to arrow_s01e01.mkv. format
		name = folderlocation + "/" + showname + "_S" + season + "E" + episode + "." + extension;
		util::toCamelCase(name);

	}

	return name;
}

QSqlQuery reconcile::get_shows(){

	QString source = settings::load_setting("source","");
	QString querystring = "Select * from shows where path like '%"+source+"'%";
	QSqlQuery results = databasehandler::execquery(querystring,reconcile_connection);
	return results;

}

bool reconcile::rename_show(QString currentpath, QString newpath){

	QFile showfile(currentpath);
	bool s = showfile.rename(newpath);
	return s;

}


void reconcile::create_ui(){
	connect(&exec_watcher, SIGNAL(finished()), this, SLOT(exec_finished()));


	main_layout = new QGridLayout;

	execute_button = new QPushButton;

	count_label = new QLabel;

	count_label->setText("Press button to begin");

	execute_button->setText("Execute");

	this->setLayout(main_layout);

	main_layout->addWidget(execute_button,1,0);

	connect(execute_button, SIGNAL(clicked()), this, SLOT(exec()));

}

reconcile::~reconcile(){
	delete execute_button;
	delete main_layout;
}

void reconcile::exec(){
	exec_future = QtConcurrent::run(async_exec);
	exec_watcher.setFuture(exec_future);
	execute_button->setDisabled(true);
}

void reconcile::async_exec(){
	QSqlQuery results = get_shows();
	std::vector<QString> original_paths;

	if (results.isValid()){

		do {

			original_paths.push_back(results.value("path").toString());

		} while (results.next());
	}

	int i = 0;
	int j = 0;
	for (auto path : original_paths){
		QString basepath = "";
		QString new_name = create_name(path, basepath);
		QFile f(QDir::toNativeSeparators(path));

		QDir dir(basepath);
		if(!dir.exists())
			dir.mkpath(".");


		if(f.rename(new_name)){
			QString query = "update shows set path = '" + new_name + "' where path = '"+path+"'";
			QSqlQuery result =  databasehandler::execquery(query, reconcile_connection);
			if (result.lastError().type() != result.lastError().NoError){
				qDebug() << result.lastError() << endl;
			}
			j++;
		} else {
			qDebug() << " could not rename " << path << " to " << new_name  <<":"<< f.errorString() << endl;
		}
		i++;
	}
}

void reconcile::exec_finished(){
	execute_button->setEnabled(true);
	emit execute_finished();
}
