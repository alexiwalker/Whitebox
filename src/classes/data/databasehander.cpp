#include "classes\data\databasehandler.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include <QSqlQuery>

QString databasehandler::defaultDB = "data.db";
databasehandler::databasehandler(QString name)
{


}


databasehandler::~databasehandler(){
}

void databasehandler::insertdata(QString table, QJsonObject values, QString connection){
	QString query = "Insert into shows (path, showname,season, episode,lastwatch,playcount) VALUES ("
	"\""+values["path"].toString() + "\","
	"\""+values["showname"].toString() + "\","
	""+QString::number(values["season"].toInt()) + ","
	""+QString::number(values["episode"].toInt()) + ","
	""+QString::number(values["lastwatch"].toInt()) + ","
	""+QString::number(values["playcount"].toInt()) + ""
	"); ";

	QSqlQuery result = execquery(query,connection);
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

void databasehandler::updatedata(QString table, QJsonObject values, QString connection){

}

QString databasehandler::querydata(QString query, QString column, QString connection){
	QSqlDatabase::removeDatabase(connection);
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE",connection);
	db.setDatabaseName(databasehandler::defaultDB);
	QString qresult = "";
	if(db.open()){
		QSqlQuery result = db.exec(query);
		result.first();
		qresult = result.value(column).toString();
	}
	else{

	}


	return qresult;
}

QSqlQuery databasehandler::execquery(QString query, QString connection){

	QSqlDatabase::removeDatabase(connection);
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE",connection);
	db.setDatabaseName(databasehandler::defaultDB);

	if(db.open()){
		QSqlQuery result = db.exec(query);
		result.first();
		return result;
	}
	return QSqlQuery();

}


void databasehandler::initdb(){
	QSqlDatabase::removeDatabase(DB_STANDARD_CONNECTION);
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE",DB_STANDARD_CONNECTION);
	db.setDatabaseName(databasehandler::defaultDB);
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
		`showname` text NOT NULL,\
		`season` int(2) NOT NULL,\
		`episode` int(2) NOT NULL,\
		`when` timestamp NOT NULL,\
		`duration` int default 0,\
		`progress` int default 0 )");

		db.exec ("CREATE TABLE IF NOT EXISTS `auto_rename` ("
		"`from` text not null, "
		"`to` text not null)");

		db.exec("CREATE TABLE IF NOT EXISTS `magnets` ("
		"`show` text not null,"
		"`season` int not null,"
		"`episode` int not null,"
		"`magnet` int not null,"
		")");

	}
}

QString databasehandler::DB_STANDARD_CONNECTION = "connection";
