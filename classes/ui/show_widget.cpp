#include "show_widget.h"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QtCore>
#include <QWidget>
#include <QFile>
show_widget::show_widget(QString show)
{
	showname = show;
	layout = new QGridLayout;
	layout2 = new QGridLayout;
	innerwidget = new QWidget;
	innerwidget->setLayout(layout);
	this->setLayout(layout2);
	layout2->setMargin(0);
	this->setFixedSize(150,150);
	this->setStyleSheet("background-color: #b2ccf7");
	this->innerwidget->setStyleSheet(not_selected_style);
	this->setProperty("showname",showname);
	layout2->addWidget(innerwidget);


	showbanner = new QWidget;
	QString formaturl = QDir::toNativeSeparators("C:\\Users\\Alexa\\PycharmProjects\\impling\\images\\the 100\\The-100-Cast-Promos-the-100-tv-show-37080703-1024-768.png").replace("\\","/");
	showbanner->setObjectName("showbanner");
	showbanner->setStyleSheet("#showbanner{border-image: url("+formaturl+");border-width:0px;border-radius:0px}");
	showbanner->setFixedSize(125,125);
//	layout->addWidget(showbanner,0,0);
//	layout->setAlignment(showbanner,Qt::AlignCenter);

	mainlabel = new QLabel;
	mainlabel->setText(showname);
	mainlabel->setAlignment(Qt::AlignCenter);
	mainlabel->setWordWrap(true);
	layout->addWidget(mainlabel,1,0);

	playbutton = new QPushButton;
	playbutton->setObjectName(showname);
	layout->addWidget(playbutton);
	playbutton->setStyleSheet("border:0px");

	playbutton->setVisible(false);

	resume = new QPushButton;
	resume->setObjectName("resume");

	resume->setProperty("showname",showname);
	resume->setText(">>");
	resume->setIcon(QIcon::fromTheme("play"));

	restart = new QPushButton;
	restart->setObjectName("restart");
	restart->setProperty("showname",showname);
	restart->setText("restart");

	layout->addWidget(resume,4,0);
//	layout->addWidget(restart,3,1);

}


bool show_widget::event(QEvent* event){

	if(event->type() == QEvent::Enter){
		this->innerwidget->setStyleSheet(selected_style);
	}

	if(event->type() == QEvent::Leave){
		this->innerwidget->setStyleSheet(not_selected_style);
	}

	if(event->type() == QEvent::MouseButtonPress){
		emit this->playbutton->clicked();
	}

	return true;
}

QString show_widget::not_selected_style =
".QWidget{"
"border-style: solid;"
"border-width: 1px;"
"border-color: black;} "
".QLabel{"
"font-size:12px;"
"}";


QString show_widget::selected_style =
".QWidget{"
"border-style: solid;"
"border-width: 3px;"
"border-color: white;"
"border-radius: 10px}"
".QLabel{"
"font-size:12px;"
"}";

