#include "player_playlist.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QGridLayout>
#include <QListView>
#include <QDebug>


player_playlist::player_playlist(QMediaPlaylist* list)
{

	this->setMinimumSize(100,250);
	this->playlist = list;
	this->layout = new QGridLayout;
	this->listview = new QListView;
	connect(this->playlist,SIGNAL(mediaInserted(int,int)) ,this,SLOT(list_update(int,int)));
	this->hide();

}

void player_playlist::list_update(int start,int end){
//	qDebug() << start << " " << end << endl;
}

void player_playlist::toggle(){
	if(this->isHidden()){
		this->show();
	} else {
		this->hide();
	}
}




