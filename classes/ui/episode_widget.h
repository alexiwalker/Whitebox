#ifndef episode_widget_H
#define episode_widget_H

#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>

class episode_widget : public QWidget
	{
public:
	episode_widget(QString showname, QString path, int season, int episode, int fileduration, int lastprogress);
	QPushButton* playlist_add;
	QPushButton* episode_play;
	QGridLayout* widget_layout;
	QProgressBar* watch_progress;
	QLabel* main_label;
	bool event(QEvent* event);
	static QString not_selected_style;
	static QString selected_style;
	static QString selected_not_availble_style;
	static QString not_selected_not_avaialable_style;
	static QString progess_bar_style;
	};

#endif // episode_widget_H
