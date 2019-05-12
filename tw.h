#ifndef TW_H
#define TW_H

#include <QMainWindow>
#include <QFutureWatcher>
#include <QFuture>
#include <QListWidget>
#include <QLineEdit>

/**
 *	QFutureWatcher<bool> watcher;
	connect(&watcher, SIGNAL(finished()), this, SLOT(async_finished()));

	QFuture<bool> future = QtConcurrent::run(async_index,source);
 */

namespace Ui {
	class tw;
}

class tw : public QMainWindow
{
		Q_OBJECT

	public:
		explicit tw(QWidget *parent = nullptr);
		~tw();

public slots:
	void build_index_finished();
	void show_rename_finished();

private slots:
	/** All offthreaded functions should be names async_function **/

	static bool async_index(QString location,QString location2);

	void build_index();

	void on_build_index_clicked();



	static bool async_rename(QString from, QString to);

	void show_rename();

	void on_show_rename_clicked();
	void show_rename_run();

	void on_dbg_clicked();

	private:
		Ui::tw *ui;

		void savesettings(QString key, int value);
		void savesettings(QString key, QString value);
		QString loadsettings(QString key, QString defaultval);
		QString autoload = "";
		QString library = "";
		QString source = "";
		void closeEvent (QCloseEvent *event);

		QFuture<bool> build_index_future;
		QFutureWatcher<bool> build_index_watcher;


		QFuture<bool> show_rename_future;
		QFutureWatcher<bool> show_rename_watcher;

		struct episode_information{
				QString showname;
				QString path;
				int season;
				int episode;
		};

		QLineEdit* newname;
		QListWidget* showlistfrom;
		QListWidget* showlistto;
public:
		void index_public_button();


};

#endif // TW_H
