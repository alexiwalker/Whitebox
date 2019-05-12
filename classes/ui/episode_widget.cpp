#include "episode_widget.h"
#include <QtCore>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QFile>
#include <QPushButton>
#include <QSqlQuery>
#include "classes/data/databasehandler.h"
#define	EPISODE_SELECTED episode_widget::selected_style
#define	EPISODE_NOT_SELECTED episode_widget::not_selected_style
#define EPISODE_SELECTED_NOT_AVAILABLE episode_widget::selected_not_availble_style
#define EPISODE_NOT_SELECTED_NOT_AVAILABLE episode_widget::not_selected_not_avaialable_style
#define PROGRESS_BAR_STYLE  episode_widget::progess_bar_style

/**
 * @brief ui_episode_widget::ui_episode_widget
 * @param showname
 * @param path
 * @param season
 * @param episode
 * @param playbutton
 * @param addbutton
 *
 *
 * 	QPushButton* playlist_add;
	QPushButton* episode_play;
	QGridLayout* widget_layout;
	bool event(QEvent* event);
 */

episode_widget::episode_widget(QString showname, QString path, int season, int episode,int fileduration, int lastprogress)
{
	this->setFixedSize(100,100);
	this->setObjectName(path);
	widget_layout = new QGridLayout;
	widget_layout->setMargin(0);
	this->setLayout(widget_layout);
	this->setProperty("path",path);
	this->setProperty("showname",showname);
	this->setProperty("season",season);
	this->setProperty("episode",episode);

	bool filexists = false;
	QFile mediafile(path);
	if(mediafile.exists())
		filexists=true;

	episode_play = new QPushButton;
	episode_play->setText(">");
	episode_play->setProperty("path",path);
	episode_play->setProperty("showname",showname);
	episode_play->setProperty("season",season);
	episode_play->setProperty("episode",episode);

	playlist_add = new QPushButton;
	playlist_add->setText("+");
	playlist_add->setProperty("path",path);
	playlist_add->setProperty("showname",showname);
	playlist_add->setProperty("season",season);
	playlist_add->setProperty("episode",episode);

	watch_progress = new QProgressBar;
	watch_progress->setTextVisible(false);
	watch_progress->setMaximumSize(100,10);
	watch_progress->setStyleSheet(PROGRESS_BAR_STYLE);
	watch_progress->setAlignment(Qt::AlignHCenter);

	if (fileduration > 0 && lastprogress > 0){
		watch_progress->setMaximum(fileduration);
		watch_progress->setValue(lastprogress);
	} else {
		watch_progress->setMaximum(100);
		watch_progress->setValue(0);
	}

	main_label = new QLabel;
	main_label->setText(QString::number(episode));
	main_label->setAlignment(Qt::AlignCenter);
	widget_layout->addWidget(main_label,0,0,0,2);
	widget_layout->addWidget(episode_play,1,0,1,1);
	widget_layout->addWidget(playlist_add,1,1,1,1);
	widget_layout->addWidget(watch_progress,2,0,1,0);

	this->setProperty("available",false);
	if(filexists){
		this->setProperty("available",true);
		this->setStyleSheet(EPISODE_NOT_SELECTED);
	}else{
		this->setProperty("available",false);
		this->setStyleSheet(EPISODE_NOT_SELECTED_NOT_AVAILABLE);
	}


}

bool episode_widget::event(QEvent *event){

	if(event->type() == QEvent::Enter){
		if(this->property("available").toBool()){
			this->setStyleSheet(EPISODE_SELECTED);
		}else{
			this->setStyleSheet(EPISODE_SELECTED_NOT_AVAILABLE);
		}
	}

	if(event->type() == QEvent::Leave){
		if(this->property("available").toBool()){
			this->setStyleSheet(EPISODE_NOT_SELECTED);

		}else{
			this->setStyleSheet(EPISODE_NOT_SELECTED_NOT_AVAILABLE);
		}
	}

	return true;

}

QString episode_widget::not_selected_style =
".QWidget{background-color: #b2ccf7;"\
"border-style: none;"\
"border-width: 0px;"\
"border-color: white;}";


QString episode_widget::selected_style =
".QWidget{background-color: #b2ccf7;"\
"border-style: outset;"\
"border-width:"\
"3px; border-color:white}";


QString episode_widget::not_selected_not_avaialable_style =
".QWidget{background-color: #d6d6d6;"\
"border-style: none;"\
"border-width: 0px;"\
"border-color: white;}";

QString episode_widget::selected_not_availble_style =
".QWidget{background-color: #16d6d6;"\
"border-style: outset;"\
"border-width: 3px;"\
"border-color: white;} ";


QString episode_widget::progess_bar_style =
"QProgressBar {\
border: 1px solid black;\
text-align: center;\
}\
QProgressBar::chunk {\
	background-color: #3add36;\
width: 1px;\
}";
