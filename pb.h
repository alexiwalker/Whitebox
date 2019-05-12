#ifndef PB_H
#define PB_H

/**
  Playback Window - plays media loaded into it by wb.h
  Does not serve as an object constructed for a media item and deleted when it ends
  But as a shell for paths to be passed to for it to render
  controlled by wb.h via pointers to the media controls
  */
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMouseEvent>
#include <QSlider>
#include <QPushButton>
#include <classes/process/microtimer.h>
#include <classes/ui/player_playlist.h>

namespace Ui {
	class pb;
}

class pb : public QMainWindow
{
		Q_OBJECT

	public:
		explicit pb(QWidget *parent = nullptr);
		~pb();
		QMediaPlayer* player;
		QMediaPlaylist* playlist;
		QVideoWidget*  videowidget;

		void player_url_play(QString path);
		void player_url_add(QString path);




		void window_close();

		struct player_control_elements{
				QSlider* trackbar;
				QPushButton* playertogglebutton;
				QPushButton* fullscreenbutton;
				QSlider* volumebar;
				QPushButton* playlistnext;
				QPushButton* listtogglebutton;
		};

		player_control_elements* controls;


		QWidget* player_controls;
		player_playlist* playlist_view;

		QString resumepath = "";
		int resumeposition = 0;

	private slots:
		void player_play();
		void player_stop();
		void player_seek(int location);
		void player_next();
		void fullscreen_toggle();
		void player_previous();
		void player_change_media(int index);
		void player_toggle();
		void player_volume(int value);
		void mediaChanged(qint64 duration);
		void check_controls(qint64 position);
		void fire_toggle(bool event);

	private:
		Ui::pb *ui;
		int lastmousemovement;
		microtimer mousetimetracker;
		bool event(QEvent *event);


};

#endif // PB_H
