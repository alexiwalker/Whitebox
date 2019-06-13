#ifndef PLAYER_PLAYLIST_H
#define PLAYER_PLAYLIST_H

#include <QObject>
#include <QWidget>
#include <QtMultimediaWidgets>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QGridLayout>
#include <QListView>

class player_playlist : public QWidget
	{

	Q_OBJECT
public:
	player_playlist(QMediaPlaylist* list);
	void toggle();


private:
	QMediaPlaylist* playlist;
	QGridLayout* layout;
	QListView* listview;

private slots:
	void list_update(int start,int end);

	};

#endif // PLAYER_PLAYLIST_H
