#include <QCoreApplication>
#include <QFile>
#include <QtConcurrent/QtConcurrent>
#include <QFileInfo>
#include <QtSql/QSql>
#include <qsqlquery>
#include <QDebug>
#include <regex>
#include <iostream>
#include "databasehandler.h"


bool extract_episode_details(QString showpath, QString &showname, int &season, int &episode){

	/// this function pulls the show name, season number and episode number from the path provided
	///
	/// each method of determining these details are pulled a case in a switch, ordered by likelyhood.
	///
	/// returns true if all 3 components were successfully pulled, otherwise false (may have determined some or none of the above reference variables)
	///
	/// before using sn,season,episode, always check if(extract_episode_details(showpath, showname, season, episode)
	///
	/// episode definitions can be easily expanded tro more unlikely patterns by adding more cases

	QFile file(showpath);

	QFileInfo fileinfo(file.fileName());


	if(fileinfo.suffix()=="nfo" || fileinfo.suffix()=="txt" || fileinfo.suffix() == "rar" || fileinfo.suffix() == ""){
		return false;
	}

	std::regex rgx("((([A-z]+[\\.|\\s|_])+)+([sS][\\d]+[eE][\\d]+))"); //arrow.s01e07
	std::regex rgx2("((([A-z]+[\\.|\\s|_])+[0-9]+[\\.|\\s|_])+([sS][\\d]+[eE][\\d]+))"); //the.100.s01e02
	std::regex rgx_seas_ep("(([sS][\\d]+)([eE][\\d]+))"); //s01e02
	std::regex rgx_seas("([sS][\\d]+)"); //s01
	std::regex rgx_ep("([eE][\\d]+)"); //e02
	std::regex rgx_num("([0-9]+)"); //01, 02

	std::smatch match;
	std::string filename = fileinfo.fileName().toStdString();
	std::string filepath = file.fileName().toStdString();

	/// STANDARD PATTERN
	///
	///
	///
	///
	/// THIS SHOULD BE HIT THE MOST OFTEN

	//arrow.s01e07.mkv
	if(
	std::regex_search(filename, match, rgx) ||
	std::regex_search(filename, match, rgx2)
	){

		std::string stringmatch = match[0];
		QString qmatch = QString::fromStdString(stringmatch);
		std::smatch seasonmatch;
		std::smatch episodematch;
		std::smatch seasonepisodematch;


		if(
		std::regex_search(stringmatch, seasonepisodematch, rgx_seas_ep)
		){
			std::string stringmatch = seasonepisodematch[0];

			if (std::regex_search(stringmatch, seasonmatch, rgx_seas) && std::regex_search(stringmatch, episodematch, rgx_ep)){

				std::string seasonstring = seasonmatch[0];
				std::string episodestring = episodematch[0];
				std::smatch seasonsubstring;
				std::smatch episodesubstring;

				std::smatch shownamematch;
				std::regex nameregex("^(.*?)(?=(.[sS][\\d]+)([eE][\\d]+))");

				if (std::regex_search(filename, shownamematch, nameregex)){
					showname = QString::fromStdString(shownamematch[0]);
					showname.replace(".", " ");
				}

				if (std::regex_search(seasonstring, seasonsubstring, rgx_num) && std::regex_search(episodestring, episodesubstring, rgx_num)){

					season = QString::fromStdString(seasonsubstring[0]).toInt() ;
					episode = QString::fromStdString(episodesubstring[0]).toInt() ;
				}
			}
		}

		return true;
	}

	///THIS IS HIT WHEN THE FILE PATH HAS THE CORRECT REGEX BUT THE FILE ITSELF DOESNT (SEEMS TO HAPPEN FROM TPB?
	///
	///
	///
	///
	///
	///
	/// NOT SURE. NEEDS TO  RENAME THE FILE AND DO THE FIRST SECTION
	//arrow.s01e07/arrow.107.mkv (path has correct regex, filename doesnt)
	if (
	std::regex_search(filepath, match, rgx) ||
	std::regex_search(filepath, match, rgx2)
	){

		std::string stringmatch = match[0];
		QString qmatch = QString::fromStdString(stringmatch);
		qDebug() << qmatch << endl;

		//find s/e/sn
		//rename
		//extract_episode_details()

		return true;
	}


	/// THIS IS HIT WHEN THERE IS SHOW NAME / SEASON X / showname_x01 (101, 201, etc)
	/// regex -> remove showname based on folder before season.whatever
	/// remove showname from filename before that, remove the number from season from the 101, then use the 01 as episode number
	///
	///
	///
	///
	///
	///
	///
	///
	/// might have issues for season >10, will need to find an example

	return false;
}

void searchrecursive(QString location){
	if (location != "") {

		qDebug() << location << endl;
		QDirIterator iterator(location, QDirIterator::Subdirectories);

		int i = 0;
		while (iterator.hasNext()) {
			QString fname = iterator.next();
			QFileInfo fileinfo(fname);
			if (fileinfo.suffix() == "" || fileinfo.suffix() == "nfo")
				continue;

			QJsonObject object = QJsonObject();

			QString showname = "";
			int season;
			int episode;

			if (extract_episode_details(fileinfo.absoluteFilePath(), showname, season, episode)) {

				qDebug() << showname << season << episode << endl;
				object["path"] = fileinfo.absoluteFilePath();
				object["lastwatch"] = NULL;
				object["playcount"] = 0;
				object["showname"] = showname;
				object["season"] = season;
				object["episode"] = episode;

				databasehandler::insertdata("shows", object);

				i++;

			}
		}
	}

}

QString loadsettings(QString key, QString defaultval) {
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");

	QSettings settings("..\\..\\settings.ini",QSettings::IniFormat);
	return settings.value(key, defaultval).toString();
}


int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QString source = loadsettings("source","");
	QString library = loadsettings("library","");

	QtConcurrent::run(searchrecursive,source);
	QtConcurrent::run(searchrecursive,library);

	return a.exec();
}
