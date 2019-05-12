#ifndef EPISODE_WIDGET_H
#define EPISODE_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>

class show_widget : public QWidget
	{

	Q_OBJECT

public:
	show_widget(QString showname);
	QString showname;
	QPushButton* playbutton;
	QString lastwatched = "";
	QGridLayout* layout;
	QGridLayout* layout2;
	QWidget* innerwidget;
	QLabel* mainlabel;
	QPushButton* restart;
	QPushButton* resume;

	static QWidget* episodewidget(QString showname, QString path, int season, int episode,QPushButton* playbutton, QPushButton* addbutton);
	static QWidget* showwidget(QString showname, QPushButton* playbutton);
	static QString not_selected_style;
	static QString selected_style;

private:
	bool event(QEvent*);
	};

#endif // EPISODE_WIDGET_H
