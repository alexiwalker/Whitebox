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

			QFuture<void> exec_future;
			QFutureWatcher<void> exec_watcher;
			static void async_exec();
			static QString replace_connection;
			private slots:
			void exec();
			void exec_finished();

		signals:
			void execute_finished();

};

#endif // REPLACE_H
