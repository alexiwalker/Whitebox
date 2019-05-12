#include "wb.h"
#include "ui_wb.h"
#include "sw.h"
#include "tw.h"
#include <QDirIterator>
#include <QResizeEvent>
#include <QScrollArea>
#include <QPushButton>
#include <QEventLoop>
#include <QLabel>
#include <QScrollBar>
#include <QSettings>
#include <QDebug>
#include <regex>
#include <chrono>
#include <QFile>
#include <QUrl>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QSizePolicy>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <classes/data/databasehandler.h>
#include <classes/process/microtimer.h>
#include <classes/process/linked_mem_buffer.h>
#include <classes/ui/flowlayout.h>
#include <classes/ui/show_widget.h>
#include <classes/ui/episode_widget.h>
#include <classes/ui/player_playlist.h>


wb::wb(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::wb)
{
	ui->setupUi(this);
	databasehandler::initdb();
	on_actionRefresh_triggered();

	this->setMinimumWidth(200);
	this->setMinimumHeight(200);
	ui->c_showdata->setMaximumHeight(0);

	int state = loadsettings("auto","0").toInt();
	if(state){
		if(toolswindow == nullptr){
			toolswindow = new tw;
		}
		if(toolswindow->isVisible())
			toolswindow->hide();

		toolswindow->index_public_button();
	}

	connect(&file_handle_watcher, SIGNAL(finished()), this, SLOT(file_handle_found()));
	file_handle_future = QtConcurrent::run(wb::file_handle_memory);
	file_handle_watcher.setFuture(file_handle_future);
}

wb::~wb()
{
	delete ui;
}

void wb::closeEvent (QCloseEvent *event){
	delete settingswindow;
	delete toolswindow;
	delete playerwindow;
}

QString wb::file_handle_memory(){
	QString read = "";
	linked_mem_buffer mem(linked_mem_buffer::SINGLE_INSTANCE,false);
	if(mem.exists())
		mem.attach();
	while(1){
		read = mem.read();
		if(read != ""){
			mem.clear();
			return read;
		}
	}
}

void wb::file_handle_found(){
	QString result = file_handle_watcher.result();
	file_handle_future = QtConcurrent::run(wb::file_handle_memory);
	file_handle_watcher.setFuture(file_handle_future);
	set_file_open_handle(result);
}

void wb::set_file_open_handle(QString file){
	QString path = file;
	QFile mediafile(path);
	if(!mediafile.exists()){

		QString message = path + " not found. Episode could not be added.";
		QMessageBox::critical(
					this,
					tr("WhiteBox"),
					tr(message.toStdString().c_str()));
		return;
	}

	QString show;
	int season;
	int episode;
	extract_episode_details(path,show,season,episode);

	QString query = "Select * from shows where path = '" + path + "'";
	QSqlQuery result = databasehandler::execquery(query);
	if (!result.isValid()){

		QJsonObject object;

		object["path"] = path;
		object["lastwatch"] = NULL;
		object["playcount"] = 0;
		object["showname"] = show;
		object["season"] = season;
		object["episode"] = episode;

		databasehandler::insertdata("shows", object);
		//this may produce an sql error if the episode already exists (show/season/episode composite key violation),
		//but it doesn't matter - if the path doesn't exist, we should try anyway

	}

	if(playerwindow == nullptr){
		playerwindow= new pb;
	}

	if(playerwindow->isHidden()){
		delete playerwindow;
		playerwindow = new pb;
	}
	playerwindow->show();

	playerwindow->player_url_add(path);
}

void wb::on_actionTools_triggered()
{
	if(toolswindow == nullptr){
		toolswindow = new tw;
		toolswindow->show();
	} else {
		toolswindow->show();
	}
}

void wb::on_actionsettings_triggered()
{
	if(settingswindow == nullptr){
		settingswindow = new sw;
		settingswindow->show();
	} else {
		settingswindow->show();
	}
}


void wb::episode_play(){
	QObject* sender = QObject::sender();

	QPushButton* button = qobject_cast<QPushButton*>(sender);

	QString path = button->property("path").toString();

	QFile mediafile(path);

	if(!mediafile.exists()){

		QString message = path + " not found. Episode could not be played.";
		QMessageBox::critical(
					this,
					tr("WhiteBox"),
					tr(message.toStdString().c_str()));
		return;
	}

	if(playerwindow == nullptr){
		playerwindow= new pb;
	}

	if(playerwindow->isHidden()){
		delete playerwindow;
		playerwindow = new pb;
	}

	playerwindow->show();
	playerwindow->player_url_play(path);

}

void wb::playlist_add(){
	QObject* sender = QObject::sender();

	QPushButton* button = qobject_cast<QPushButton*>(sender);
	QString path = button->property("path").toString();

	QFile mediafile(path);
	if(!mediafile.exists()){

		QString message = path + " not found. Episode could not be added.";
		QMessageBox::critical(
					this,
					tr("WhiteBox"),
					tr(message.toStdString().c_str()));
		return;
	}


	if(playerwindow == nullptr){
		playerwindow= new pb;
	}

	if(playerwindow->isHidden()){
		delete playerwindow;
		playerwindow = new pb;
	}
	playerwindow->show();

	playerwindow->player_url_add(path);

}


void wb::savesettings(QString key, QString value) {
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");
	QSettings settings("settings.ini",QSettings::IniFormat);
	settings.setValue(key, value);
}

void wb::savesettings(QString key, int value) {
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");
	QSettings settings("settings.ini",QSettings::IniFormat);
	settings.setValue(key, value);
}

QString wb::loadsettings(QString key, QString defaultval) {
	QCoreApplication::setOrganizationName("Walkersoft");
	QCoreApplication::setApplicationName("whitebox");
	QSettings settings("settings.ini",QSettings::IniFormat);
	return settings.value(key, defaultval).toString();
}

void wb::clearlayout(QLayout* layout){
	if ( layout != nullptr )
	{
		QLayoutItem* item;
		while ( ( item = layout->takeAt( 0 ) ) != nullptr )
		{
			layout->removeItem(item);
			layout->removeWidget(item->widget());
		}
	}

}

void wb::deleteLayoutContents(QLayout* layout){
	if ( layout != nullptr )
	{
		QLayoutItem* item;
		while ( ( item = layout->takeAt( 0 ) ) != nullptr )
		{
			delete item->widget();
			delete item;
		}
	}
}

void wb::ui_name_clicked(){

	deleteLayoutContents(ui->c_seasonlist);

	QObject* sender = QObject::sender();

	QWidget* titlewidget = new QWidget;

	QLabel* showlabel = new QLabel;

	QGridLayout* basegridlayout = new QGridLayout;
	QString showname = sender->objectName();
	showlabel->setText(showname);
	basegridlayout->setSpacing(2);
	titlewidget->setLayout(basegridlayout);
	basegridlayout->addWidget(showlabel,0,0);
	ui->c_seasonlist->addWidget(titlewidget);
	ui->c_seasonlist->setAlignment(Qt::AlignCenter);
	titlewidget->setMaximumHeight(45);

	//	titlewidget->setStyleSheet("background:blue");
	titlewidget->setSizePolicy(QSizePolicy::Expanding , QSizePolicy::Fixed);

	QWidget* basewidget = new QWidget;
	QGridLayout* backgroundgrid = new QGridLayout;
	//	backgroundgrid->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	basewidget->setLayout(backgroundgrid);
	basegridlayout->addWidget(basewidget,1,0);

	QString query = "select distinct `season` from `shows` where `showname` = \'"+showname+"\';";
	QSqlQuery result = databasehandler::execquery(query);
	QComboBox *comboBox = new QComboBox;
	comboBox->setMinimumSize(125,25);
	showlabel->setMinimumSize(125,25);
	showlabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

	comboBox->setProperty("show", showname);

	comboBox->setStyleSheet ("QComboBox::drop-down {border-width: 0px;text-align:center} QComboBox::down-arrow {image: url(noimg); border-width: 0px;} textalign:;");

	//Doing the addItem inside the initial do{}while() caused a query overwrite in sqlite memory
	//because adding the first item -> triggered box changed -> loaded a new query into memory, which meant only the first
	//season was being loaded. Storing in vector and iterating after solves this
	//other potential: load first box item as "recommended", which resumes the show, recommends first episode if not watched etc
	connect(comboBox,SIGNAL(currentIndexChanged(int)), this, SLOT(box_change(int)));

	basegridlayout->addWidget(comboBox,0,1);
	std::vector<std::string> seasons;

	do {
		QString showname = result.value("season").toString();
		seasons.push_back(showname.toStdString());

	} while (result.next());

	for(auto const& value: seasons)
		comboBox->addItem(QString::fromStdString(value));

	backgroundgrid->setAlignment(Qt::AlignHCenter);
}

void wb::show_play_resume(){
	QObject* sender = QObject::sender();

	QString showname = sender->property("showname").toString();
	if(playerwindow == nullptr){
		playerwindow= new pb;
	}

	if(playerwindow->isHidden()){
		delete playerwindow;
		playerwindow = new pb;
	}

	playerwindow->show();

	playerwindow->playlist->clear();
	QString query = "select `path` from watchhistory where `path` in (select `path` from shows where `showname`= '"+showname+"') order by `when` desc limit 1";

	QSqlQuery result = databasehandler::execquery(query);

	QString mostrecent = result.value("path").toString();

	QString query2 = "select * from shows where path = '"+mostrecent+"'";

	result = databasehandler::execquery(query2);
	int escore =0;

	if(result.isValid()){
		do {
			escore= result.value("episode").toInt() + (result.value("season").toInt()*10);
			playerwindow->resumepath = result.value("path").toString();
			playerwindow->resumeposition = result.value("lastprogress").toInt() > 3000 ? result.value("lastprogress").toInt()-3000 : result.value("lastprogress").toInt(); //3 second buffer for resume if it is >3 secs in
		} while (result.next());
	}


	QString query3 = "select * from `shows` where `showname` = '"+showname+"' order by season asc, episode asc";
	result = databasehandler::execquery(query3);

	if(result.isValid()){
		do {
			int cscore = result.value("episode").toInt() + (result.value("season").toInt()*10);
			if (cscore >= escore){
				playerwindow->player_url_add(result.value("path").toString());
			}
		} while (result.next());
	}

	playerwindow->player->play();
}

void wb::show_play_restart(){

	QObject* sender = QObject::sender();
	QString showname = sender->property("showname").toString();

	if(playerwindow == nullptr){
		playerwindow= new pb;
	}

	if(playerwindow->isHidden()){
		delete playerwindow;
		playerwindow = new pb;
	}

	playerwindow->show();

	playerwindow->playlist->clear();
	QString query = "select * from shows where showname = '"+showname+"' order by season asc, episode asc";
	QSqlQuery result = databasehandler::execquery(query);

	do {
		playerwindow->player_url_add(result.value("path").toString());

	} while (result.next());

}

void wb::box_change(int index){

	deleteLayoutContents(ui->d_episodelist);
	QObject* sender = QObject::sender();

	QComboBox* q = qobject_cast<QComboBox*>(sender);
	int season = q->itemText(index).toInt();
	QString showname = sender->property("show").toString();

	QString query = "select * from shows where showname = \"" + showname + "\" and season = " +QString::number(season)+";";
	QSqlQuery result = databasehandler::execquery(query);
	int i = 0;

	QScrollArea* scrollarea = new QScrollArea;
	episode_scroll_bar = scrollarea->horizontalScrollBar();
	QGridLayout* episodegrid = new QGridLayout;
	QWidget* basewidget = new QWidget;
	basewidget->setLayout(episodegrid);

	scrollarea->setStyleSheet("border:none");

	QPushButton* leftscroll = new QPushButton;
	QPushButton* rightscroll = new QPushButton;
	leftscroll->setMaximumHeight(110);
	rightscroll->setMaximumHeight(110);


	leftscroll->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
	rightscroll->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
	leftscroll->setObjectName("left");
	rightscroll->setObjectName("right");
	connect(leftscroll, SIGNAL(clicked(bool)), this, SLOT(move_episode_scroll()));
	connect(rightscroll, SIGNAL(clicked(bool)), this, SLOT(move_episode_scroll()));

	do{

		int episode = result.value("episode").toInt();
		int season = result.value("season").toInt();
		int fileduration = result.value("fileduration").toInt();
		int lastprogress = result.value("lastprogress").toInt();
		QString path = result.value("path").toString();

		episode_widget* episodeitem = new episode_widget(showname,path,season,episode, fileduration,lastprogress);

		connect(episodeitem->episode_play, SIGNAL(clicked(bool)), this, SLOT(episode_play()));
		connect(episodeitem->playlist_add, SIGNAL(clicked(bool)), this, SLOT(playlist_add()));

		episodegrid->addWidget(episodeitem,0,i);
		i++;

	} while(result.next());

	episodegrid->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	scrollarea->setWidget(basewidget);
	scrollarea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	scrollarea->QAbstractScrollArea::setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	scrollarea->QAbstractScrollArea::setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	scrollarea->setFixedHeight(125);

	scrollarea->verticalScrollBar()->setEnabled(false);
	ui->d_episodelist->addWidget(leftscroll,2,0);
	ui->d_episodelist->addWidget(scrollarea,2,1);
	ui->d_episodelist->addWidget(rightscroll,2,2);
	ui->d_episodelist->setSpacing(0);

	if(ui->c_showdata->maximumHeight() < 185){
		QPropertyAnimation *animation = new QPropertyAnimation(ui->c_showdata, "maximumHeight");
		animation->setDuration(150);
		animation->setStartValue(ui->c_showdata->height());
		animation->setEndValue(185);

		animation->start();
	}

}

void wb::move_episode_scroll(){
	QObject* sender = QObject::sender();

	if(sender->objectName() == "left"){
		episode_scroll_bar->setValue(episode_scroll_bar->value()-45);
	}

	if(sender->objectName() == "right"){
		episode_scroll_bar->setValue(episode_scroll_bar->value()+45);
	}

}


bool wb::extract_episode_details(QString showpath, QString &showname, int &season, int &episode){

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

		//find s/e/sn
		//rename
		//extract_episode_details()

		return false;
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



void wb::on_actionRefresh_triggered()
{

	delete showslist;
	delete show_flow_layout;
	delete show_base_widget;
	delete show_scroll_area;

	showslist = new std::vector<show_widget*>;
	show_base_widget = new QWidget;
	show_scroll_area = new QScrollArea;
	show_flow_layout = new FlowLayout;

	/*
	 * do not change the order of these deletes/recreate
	 * causes crashing if area is before base, or base is before flow. ensuring full deleting
	 * without allowing anything to be left in memory, for safety
	 *
	 * a small amount of efficiency is lost by declaring them on construct by default and then deleting them again but it allows
	 * them to be properly cleared and managed if they ever need to be changed/updated
	 */

	QString query = "select distinct `showname` from `shows` where showname like '%"+ui->search->text()+"%' ORDER BY showname COLLATE NOCASE";
	QSqlQuery result = databasehandler::execquery(query);
	QLabel* invalid = new QLabel;
	if(result.isValid()){
		do {
			show_widget* showwidget = new show_widget(result.value("showname").toString());
			connect(showwidget->playbutton, SIGNAL(clicked(bool)), this, SLOT(ui_name_clicked()));
			connect(showwidget->restart, SIGNAL(clicked()),this, SLOT(show_play_restart()));
			connect(showwidget->resume, SIGNAL(clicked()),this, SLOT(show_play_resume()));
			showslist->emplace_back(showwidget);
		} while (result.next());
	} else {
		invalid->setText("No Shows found");
	}



	show_scroll_area->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	show_scroll_area->QAbstractScrollArea::setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	show_scroll_area->setStyleSheet("border:none");


	ui->b_showlist->layout()->addWidget(show_scroll_area);
	ui->b_showlist->layout()->setAlignment(Qt::AlignCenter);

	if(showslist->size() > 0)
		for (show_widget* currentwidget : *showslist){
			show_flow_layout->addWidget(currentwidget);
			show_flow_layout->setAlignment(currentwidget,Qt::AlignCenter);
		}
	else
		show_flow_layout->addWidget(invalid);

	show_flow_layout->setSpacing(100);

	show_base_widget->setLayout(show_flow_layout);
	show_scroll_area->setWidget(show_base_widget);

	show_base_widget->setStyleSheet("background:#e0e0e0");
	show_scroll_area->setWidgetResizable(true);

}

void wb::on_search_textChanged(const QString &arg1){

	for(QObject* child :show_base_widget->children()){

		if(!child->isWidgetType())
			continue;

		QWidget *qw = qobject_cast<QWidget*>(child);

		if(!child->property("showname").toString().toLower().contains(arg1.toLower()))	{
			if(qw->isVisible())
				qw->hide();
		} else {
			if(qw->isHidden())
				qw->show();
		}
	}
}
