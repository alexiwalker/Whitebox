#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H
#include <QObject>
#include <QtSql>
class databasehandler
{
public:
	databasehandler(QString name = "data.db");
	static QString qnow(); //for timestamping inserts

	static void initdb(); //Contains information required to construct the database from nothing, only run if the file does not exist or is empty
	static void insertdata(QString table, QJsonObject values, QString connection=DB_STANDARD_CONNECTION);
	static void updatedata(QString table, QJsonObject values, QString connection=DB_STANDARD_CONNECTION);

	static QString querydata(QString query, QString column, QString connection=DB_STANDARD_CONNECTION); //queries must return data. update methoc shoud exec

	static QSqlQuery execquery(QString query, QString connection=DB_STANDARD_CONNECTION);


	/*
	* Most of these don't require any object context, but building the insert ignore would
/	*/
	void insert_ignore_build(QString table, QString columns, QString values);
	void insert_ignore_execute();
	QString insert_ignore = "";

	static QString DB_STANDARD_CONNECTION;

	~databasehandler();

};

#endif // DATABASEHANDLER_H
