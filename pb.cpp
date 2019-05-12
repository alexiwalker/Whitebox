#include "pb.h"
#include "ui_pb.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QProgressBar>
#include <QSlider>
#include <QMouseEvent>
#include <QPushButton>
#include "classes/data/databasehandler.h"
#include "classes/ui/player_playlist.h"
#include <QApplication>

pb::pb(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::pb)
{
	ui->setupUi(this);

	player = new QMediaPlayer;
	playlist = new QMediaPlaylist;
	videowidget = new QVideoWidget;
	playlist_view = new player_playlist(playlist);

	player->setPlaylist(playlist);
	ui->playerlayout->setMargin(0);
	player->setVideoOutput(videowidget);
	ui->playerlayout->addWidget(videowidget);
	ui->centralwidget->layout()->setMargin(0);
	player_controls = new QWidget;

	player_controls->setFixedHeight(75);
	QGridLayout* playerlayout = new QGridLayout;
	player_controls->setLayout(playerlayout);

	controls = new player_control_elements;
	controls->trackbar = new QSlider;
	controls->playertogglebutton = new QPushButton;
	controls->fullscreenbutton = new QPushButton;
	controls->volumebar = new QSlider;
	controls->playlistnext = new QPushButton;
	controls->listtogglebutton = new QPushButton;

	controls->playertogglebutton->setText("||");
	controls->fullscreenbutton->setText("Full Screen");

	controls->volumebar->setOrientation(Qt::Orientation::Vertical);
	controls->trackbar->setOrientation(Qt::Orientation::Horizontal);

	controls->volumebar->setMaximum(100);

	controls->playlistnext->setText(">>");

	playerlayout->addWidget(controls->trackbar,0,0,1,5);
	playerlayout->addWidget(controls->playertogglebutton,1,0);
	playerlayout->addWidget(controls->fullscreenbutton,1,1);
	playerlayout->addWidget(controls->volumebar,0,5,2,1);

	controls->fullscreenbutton->setFixedHeight(20);

	controls->playertogglebutton->setFixedWidth(20);
	controls->playertogglebutton->setFixedHeight(20);

	controls->volumebar->setValue(player->volume());

	controls->listtogglebutton->setText("Show Playlist");
	controls->listtogglebutton->setFixedHeight(20);
	playerlayout->addWidget(controls->listtogglebutton,1,4);

	connect(controls->fullscreenbutton, SIGNAL(clicked(bool)), this, SLOT(fullscreen_toggle()));
	connect(controls->playertogglebutton, SIGNAL(clicked(bool)), this, SLOT(player_toggle()));
	connect(controls->volumebar, SIGNAL(sliderMoved(int)), this, SLOT(player_volume(int)));
	connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(mediaChanged(qint64)));
	connect(controls->trackbar, SIGNAL(sliderMoved(int)),this,SLOT(player_seek(int)));
	connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(check_controls(qint64)));

	connect(controls->listtogglebutton,SIGNAL(clicked(bool)),this,SLOT(fire_toggle(bool)));


//	ui->playerlayout->addWidget(player_controls);
	player_controls->show();
	player_controls->setWindowFlag(Qt::FramelessWindowHint);
	player->setVolume(0);

}


pb::~pb()
{
	player->stop();
	delete player_controls;
	delete ui;
}


void pb::fire_toggle(bool event){
	this->playlist_view->toggle();
}


void pb::check_controls(qint64 position){
		//checks if the file has been loaded properly and is seekable before trying to change it, or it will start at 0  every time
	if(player->isSeekable()){
		QString path = player->currentMedia().canonicalUrl().toLocalFile();

		//checks if the thing has already been resumed to position
		if (resumepath == path && resumepath != "" && resumeposition != 0){
			player->setPosition(resumeposition);
			resumeposition =0;
			resumepath="";
		}
	}

	if(position > 0 ){
		QString query = "update `shows` set `lastprogress` = " + QString::number(position) + " where `path` = '" + player->currentMedia().canonicalUrl().toLocalFile() + "'";
		controls->trackbar->setValue(position);
		QSqlQuery result = databasehandler::execquery(query );
	}

	QApplication::processEvents();
}


void pb::mediaChanged(qint64 duration){

	controls->trackbar->setMaximum(duration);

	QString path = player->currentMedia().canonicalUrl().toLocalFile();

	QString query = "update shows set \'fileduration\' = " + QString::number(duration) + " where path = \'" +path + "\'";
	databasehandler::execquery(query);

	QString query2  = "insert into watchhistory ('path','when') VALUES (\""+path+"\", "+databasehandler::qnow()+")";
	QSqlQuery result =databasehandler::execquery(query2);

	this->setWindowTitle(path);



}


void pb::window_close(){
	player_controls->hide();
	player->stop();
	this->hide();
}


void pb::fullscreen_toggle(){

	if(this->isFullScreen())
		this->showMaximized();
	else
		this->showFullScreen();

}


void pb::player_url_play(QString path){

	playlist->clear();
	playlist->addMedia(QUrl::fromLocalFile(path));
	player->play();
	controls->trackbar->setMaximum(0);
}


void pb::player_volume(int value){
	player->setVolume(value);
}


void pb::player_url_add(QString path){
	if(this->isHidden())
		this->show();

	playlist->addMedia(QUrl::fromLocalFile(path));

	if(player->state()== QMediaPlayer::StoppedState)
		player->play();

}


void pb::player_play(){
	player->play();
}


void pb::player_stop(){
	player->stop();
}

void pb::player_seek(int location){
	player->setPosition(location);
}


void pb::player_next(){
	playlist->next();
}

void pb::player_previous(){
	playlist->previous();
}


void pb::player_change_media(int index){
	playlist->setCurrentIndex(index);
}

void pb::player_toggle(){

	if(player->state() == QMediaPlayer::StoppedState || player->state()==QMediaPlayer::PausedState)
		player->play();
	else if(player->state() == QMediaPlayer::PlayingState)
		player->pause();

}


bool pb::event(QEvent *event){

	if (event->type() == QEvent::MouseButtonPress)	{
		if(player_controls->isHidden()){
			player_controls->show();
		}
		player_controls->raise();
		return true;
	}

	if (event->type() == QEvent::MouseButtonDblClick)	{
		if(!player_controls->isHidden()){
			player_controls->hide();
		}
		return true;
	}

	if(event->type() == QEvent::Close){
		window_close();
		return true;
	}

	return false;
}
