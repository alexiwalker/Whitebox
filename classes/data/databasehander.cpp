#include "classes\data\databasehandler.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include <QSqlQuery>

databasehandler::databasehandler(QString name)
{


}

databasehandler::~databasehandler(){
}

void databasehandler::insertdata(QString table, QJsonObject values){
	QString query = "Insert into shows (path, showname,season, episode,lastwatch,playcount) VALUES ("
	"\""+values["path"].toString() + "\","
	"\""+values["showname"].toString() + "\","
	""+QString::number(values["season"].toInt()) + ","
	""+QString::number(values["episode"].toInt()) + ","
	""+QString::number(values["lastwatch"].toInt()) + ","
	""+QString::number(values["playcount"].toInt()) + ""
	"); ";

	QSqlQuery result = execquery(query);
}

void databasehandler::insert_ignore_build(QString table, QString columns, QString values){

	if(insert_ignore == ""){
		insert_ignore = "Insert ignore into " + table + " (" + columns + ")	VALUES (" + values + " ),";
	} else {
		insert_ignore += "(" + values + "),";
	}

}

void databasehandler::insert_ignore_execute(){
	execquery(insert_ignore);

}

void databasehandler::updatedata(QString table, QJsonObject values){

}

QString databasehandler::querydata(QString query, QString column){
	QSqlDatabase::removeDatabase("connection");
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","connection");
	db.setDatabaseName("data.db");
	QString qresult = "";
	if(db.open()){
		QSqlQuery result = db.exec(query);
		result.first();
		qresult = result.value(column).toString();
	}
	else{
//		qDebug() << "connection failed: " << db.lastError() << endl;
	}


	return qresult;
}

QSqlQuery databasehandler::execquery(QString query){

	QSqlDatabase::removeDatabase("connection");
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","connection");
	db.setDatabaseName("data.db");

	if(db.open()){
		QSqlQuery result = db.exec(query);
		result.first();
		return result;
	}
	return QSqlQuery();

}


QString databasehandler::qnow(){

	unsigned __int64 now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	QString qnow = QString::number(now);

	return qnow;

}

void databasehandler::initdb(){
	QSqlDatabase::removeDatabase("connection");
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","connection");
	db.setDatabaseName("data.db");
	if(db.open()){
		db.exec(" CREATE TABLE IF NOT EXISTS `shows` (\
		`path` text NOT NULL,\
		`showname` text NOT NULL,\
		`season` int(2) NOT NULL,\
		`episode` int(2) NOT NULL,\
		`lastwatch` timestamp default NULL,\
		`playcount` int(3) default 0 NOT NULL, \
		`fileduration` bigint default 0 NOT NULL,\
		`lastprogress`	bigint DEFAULT 0,\
		CONSTRAINT episodedetails PRIMARY KEY (showname,season,episode)) ");

		db.exec(" CREATE TABLE IF NOT EXISTS `watchhistory` (\
		`path` text NOT NULL, \
		`when` timestamp NOT NULL,\
		`duration` int default 0,\
		`progress` int default 0 )");
	}
}
