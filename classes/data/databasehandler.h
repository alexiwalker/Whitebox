#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H
#include <QObject>
#include <QtSql>
class databasehandler
{
public:
	databasehandler(QString name = "data.db");
	static QString qnow(); //for timestamping inserts
	static void insertdata(QString table, QJsonObject values);
	static void updatedata(QString table, QJsonObject values);
	static QString querydata(QString query, QString column); //queries must return data. update methoc shoud exec
	static QSqlQuery execquery(QString query);
	static void initdb(); //Contains information required to construct the database from nothing, only run if the file does not exist or is empty
	void insert_ignore_build(QString table, QString columns, QString values);
	void insert_ignore_execute();
	QString insert_ignore = "";


	~databasehandler();

};

#endif // DATABASEHANDLER_H
