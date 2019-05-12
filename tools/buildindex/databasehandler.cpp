#include "databasehandler.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSql>
#include <QDebug>
#include <QFileInfo>
#include <QSqlQuery>
#include <QJsonObject>

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


	qDebug()<< query << endl;
	QSqlQuery result = execquery(query);
	qDebug() << result.lastError() << endl;
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
	qDebug() << db.connectionName() << endl;
	db.setDatabaseName("data.db");
	QString qresult = "";
	if(db.open()){
		QSqlQuery result = db.exec(query);
		result.first();
		qresult = result.value(column).toString();
	}
	else{
		qDebug() << "connection failed: " << db.lastError() << endl;
	}


	return qresult;
}

QSqlQuery databasehandler::execquery(QString query){

	QSqlDatabase::removeDatabase("connection");
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","connection");
	db.setDatabaseName("..\\..\\data.db");

	if(db.open()){
		QSqlQuery result = db.exec(query);
		result.first();
		return result;
	}
	qDebug() << db.lastError() << endl;
	return QSqlQuery();

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
		CONSTRAINT episodedetails PRIMARY KEY (showname,season,episode)) ");

		db.exec(" CREATE TABLE IF NOT EXISTS `watchhistory` (\
		`path` text NOT NULL, \
		`when` timestamp NOT NULL)");
	}
}
