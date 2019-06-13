#ifndef RECONCILE_H
#define RECONCILE_H

#include <QObject>
#include <QWidget>
#include <QSqlQuery>
#include <QPushButton>
#include <QGridLayout>
#include <QFuture>
#include <QFutureWatcher>
#include <QLabel>
#include <vector>

class reconcile : public QWidget
{
	Q_OBJECT
	public:
		reconcile();
		~reconcile();

	private:

		void create_ui();
		QPushButton* execute_button;
		QGridLayout* main_layout;
		QLabel* count_label;

		static QString create_name(QString file, QString &basepath);
		static QSqlQuery get_shows();
		bool  rename_show(QString currentpath, QString newpath);

		QFuture<void> exec_future;
		QFutureWatcher<void> exec_watcher;
		static void async_exec();
		static QString reconcile_connection;
		private slots:
		void exec();
		void exec_finished();

	signals:
		void execute_finished();

};

#endif // RECONCILE_H
