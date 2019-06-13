#include "playback.h"
#include "ui_playback.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QProgressBar>
#include <QSlider>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPushButton>
#include "classes/data/databasehandler.h"
#include "classes/ui/player_playlist.h"
#include <QApplication>
#include <classes/process/util.h>


QString playbackWindow::PLAYHISTORY_CONNECTION = "PLAYBACK_DATABASE_CONNECTION";

playbackWindow::playbackWindow(QWidget *parent) :
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

	connect(controls->fullscreenbutton, SIGNAL(clicked(bool)), this, SLOT(player_togglefullscreen()));
	connect(controls->playertogglebutton, SIGNAL(clicked(bool)), this, SLOT(player_toggle()));
	connect(controls->volumebar, SIGNAL(sliderMoved(int)), this, SLOT(player_volume(int)));
	connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(mediaChanged(qint64)));
	connect(controls->trackbar, SIGNAL(sliderMoved(int)),this,SLOT(player_seek(int)));
	connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(check_controls(qint64)));
	connect(player,SIGNAL(volumeChanged(int)),this,SLOT(update_vol(int)));

	connect(controls->listtogglebutton,SIGNAL(clicked(bool)),this,SLOT(fire_toggle(bool)));


	//	ui->playerlayout->addWidget(player_controls);
	player_controls->show();
}



playbackWindow::~playbackWindow()
{
	player->stop();
	delete player_controls;
	delete ui;
}


void playbackWindow::fire_toggle(bool event){
	this->playlist_view->toggle();
}

void playbackWindow::update_vol(int vol){
	controls->volumebar->setValue(vol);
}

void playbackWindow::check_controls(qint64 position){
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
		QSqlQuery result = databasehandler::execquery(query, PLAYHISTORY_CONNECTION);
	}

	controls->volumebar->setValue(player->volume());

	QApplication::processEvents();
}


void playbackWindow::mediaChanged(qint64 duration){

	controls->trackbar->setMaximum(duration);

	QString path = player->currentMedia().canonicalUrl().toLocalFile();

	QString query = "update shows set \'fileduration\' = " + QString::number(duration) + " where path = \'" +path + "\'";
	databasehandler::execquery(query, PLAYHISTORY_CONNECTION);

	query = "select * from shows where path = '"+path+"'";
	QSqlQuery result = databasehandler::execquery(query, PLAYHISTORY_CONNECTION);

	QString season,showname,episode;
	season = result.value("season").toString();
	showname = result.value("showname").toString();
	episode = result.value("episode").toString();


	QString query2  = "insert into `watchhistory` ('showname','season','episode','when') VALUES (\""+showname+"\",\""+season+"\",\""+episode+"\", "+util::qnow()+")";
	result =databasehandler::execquery(query2, PLAYHISTORY_CONNECTION);
	this->setWindowTitle(path);

}


void playbackWindow::window_close(){
	player_controls->hide();
	player->stop();
	this->hide();
}


void playbackWindow::player_togglefullscreen(){

	if(this->isFullScreen())
		this->showMaximized();
	else
		this->showFullScreen();

}


void playbackWindow::player_url_play(QString path){

	playlist->clear();
	playlist->addMedia(QUrl::fromLocalFile(path));
	player->play();
	controls->trackbar->setMaximum(0);
}


void playbackWindow::player_volume(int value){
	player->setVolume(value);
}


void playbackWindow::external_url_add(QString path){
	if(this->isHidden())
		this->show();


	player_url_play(path);
//	playlist->addMedia(QUrl::fromLocalFile(path));

//	if(player->state()== QMediaPlayer::StoppedState)
//		player->play();

	qDebug() << player->state() << endl;
}


void playbackWindow::player_play(){
	player->play();
}


void playbackWindow::player_stop(){
	player->stop();
}

void playbackWindow::player_seek(int location){
	player->setPosition(location);
}


void playbackWindow::player_next(){
	playlist->next();
}

void playbackWindow::player_previous(){
	playlist->previous();
}


void playbackWindow::player_change_media(int index){
	playlist->setCurrentIndex(index);
}

void playbackWindow::player_toggle(){

	if(player->state() == QMediaPlayer::StoppedState || player->state()==QMediaPlayer::PausedState)
		player->play();
	else if(player->state() == QMediaPlayer::PlayingState)
		player->pause();

}

void playbackWindow::player_skipforward(){
	player->setPosition(player->position()+5000);
}

void playbackWindow::player_skipbackward(){
	player->setPosition(player->position()-5000);
}

void playbackWindow::player_togglemuted(){
	player->setMuted(!player->isMuted());
}

void playbackWindow::player_volumeup(){
	player->setVolume(player->volume()+10);
	//	controls->volumebar->setValue(player->volume());
}

void playbackWindow::player_volumedown(){
	player->setVolume(player->volume()-10);
	//	controls->volumebar->setValue(player->volume());
}

void playbackWindow::player_speedup(){
	player->setPlaybackRate(player->playbackRate()+0.25);
	qDebug() << player->playbackRate() << endl;
}

void playbackWindow::player_speeddown(){

	player->setPlaybackRate(player->playbackRate()-0.25);
}

bool playbackWindow::keybind(int key){
	switch (key) {

		/** todo: move this to its own fn that allows rebinding **/

		// space key
		case 32:
			player_toggle();
			break;

			// m key
		case 77:
			player_togglemuted();
			break;

			// f key
		case 70:
			player_togglefullscreen();
			break;

			// right key
		case 16777236:
			player_skipforward();
			break;

			// left key
		case 16777234:
			player_skipbackward();
			break;

			// up key
		case 16777235:
			player_volumeup();
			break;

			// down key
		case 16777237:
			player_volumedown();
			break;

			// . key
		case 46:
			player_speedup();
			break;

			// , key
		case 44:
			player_speeddown();
			break;

			// q key
		case 81:
			window_close();
			break;

			// h key - toggle hide/show controls
		case 72:
			if(player_controls->isHidden()){
				player_controls->show();
				player_controls->raise();
			} else
				player_controls->hide();

			break;

		default:
			qDebug() << key << endl;
			return false;

	}
	return true;
}

bool playbackWindow::event(QEvent *event){
	if(event->type() == QEvent::KeyPress){
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if(!keybind(keyEvent->key()))
			qDebug() << keyEvent->key() << endl;

	}

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
