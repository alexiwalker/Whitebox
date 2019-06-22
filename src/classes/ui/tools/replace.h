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
#include <classes/process/util.h>

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
			QPushButton* openfilechoose;
			QLabel* selectedfile;
			QString selectedfilestring;
			QString selectedfilestring2;
			QString showname;
			int seasnum;
			int epnum;

			QFuture<util::success_message*> exec_future;
			QFutureWatcher<util::success_message*> exec_watcher;
			static util::success_message* async_exec(QString oldpath, QString newpath);
			static QString replace_connection;
			static bool delete_old_show(QString path);
			static bool update_database_path(QString oldpath, QString newpath);
			static bool move_new_file(QString frompath, QString topath);

			void reset_ui();

		private slots:
			void exec();
			void exec_finished();

			void loadseasons(int row);
			void loadepisodes(int row);
			void setepisode(int row);
			void choosefile();
		signals:
			void execute_finished();

};

#endif // REPLACE_H

