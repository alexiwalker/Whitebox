#ifndef REPLACE_H
#define REPLACE_H

#include <QObject>
#include <QWidget>
#include <QSqlQuery>
#include <QPushButton>
#include <QGridLayout>
#include <QFuture>
#include <QFutureWatcher>
#include <QLabel>
#include <vector>
#include <QListWidget>

class replace : public QWidget
{
		Q_OBJECT
		public:
			replace();
			~replace();

		private:
			void create_ui();
			QPushButton* execute_button;
			QGridLayout* main_layout;
			QListWidget* showlist;
			QListWidget* seasonlist;
			QListWidget* episodelist;
			QLabel* selectedepisode;

			QString showname;
			int seasnum;
			int epnum;

			QFuture<void> exec_future;
			QFutureWatcher<void> exec_watcher;
			static void async_exec();
			static QString replace_connection;
			bool delete_old_show(QString path);
			bool update_database_path(QString oldpath, QString newpath);
			bool move_new_file(QString frompath, QString topath);
	private slots:
			void exec();
			void exec_finished();

			void loadseasons(int row);
			void loadepisodes(int row);
			void setepisode(int row);
	signals:
			void execute_finished();

};

#endif // REPLACE_H

