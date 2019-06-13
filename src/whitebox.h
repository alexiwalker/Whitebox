#ifndef WB_H
#define WB_H

#include <QMainWindow>
#include <QGridLayout>
#include <QScrollArea>
#include <QMediaPlayer>
#include <QFuture>
#include <QFutureWatcher>
#include "classes/ui/flowlayout.h"
#include "classes/ui/episode_widget.h"
#include "classes/ui/show_widget.h"

#include "settingswindow.h"
#include "toolswindow.h"
#include "playback.h"

namespace Ui {
	class wb;
	}

class whiteBox : public QMainWindow
	{
	Q_OBJECT

public:
	explicit whiteBox(QWidget *parent = nullptr);
	~whiteBox();
	void set_file_open_handle(QString file);

	static bool extract_episode_details(QString showpath, QString &showname, int &season, int &episode);

private slots:

	void on_actionTools_triggered();

	void on_actionsettings_triggered();

	void ui_name_clicked();

	void episode_play();

	void playlist_add();

	void box_change(int index);

	void on_actionRefresh_triggered();

	void on_search_textChanged(const QString &arg1);

	void move_episode_scroll();

	void show_play_restart();

	void show_play_resume();

	void file_handle_found();


private:

	Ui::wb *ui;
	settingsWindow* settingswindow = nullptr;
	toolsWindow* toolswindow = nullptr;
	playbackWindow* playerwindow = nullptr;

	QMediaPlayer* player;
	QMediaPlaylist* playlist;

	QString library;
	QString source;

	void deleteLayoutContents(QLayout* layout);
	void clearlayout(QLayout* layout);

	int pw = 0;

	void closeEvent (QCloseEvent *event);

	std::vector<show_widget*>* showslist = new std::vector<show_widget*>;
	QScrollArea* show_scroll_area = new QScrollArea;
	QScrollBar* episode_scroll_bar;
	QGridLayout* show_grid_layout = new QGridLayout;
	FlowLayout* show_flow_layout = new FlowLayout;
	QWidget* show_base_widget = new QWidget;

	QFuture<QString> file_handle_future;
	QFutureWatcher<QString> file_handle_watcher;

	static QString file_handle_memory();

	};

#endif // WB_H
