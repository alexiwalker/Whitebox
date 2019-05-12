#ifndef SW_H
#define SW_H

#include <QMainWindow>

namespace Ui {
	class sw;
}

class sw : public QMainWindow
{
		Q_OBJECT

	public:
		explicit sw(QWidget *parent = nullptr);
		~sw();

	private slots:
		void on_librarybutton_clicked();

		void on_sourcebutton_clicked();

		void on_autoindex_stateChanged(int arg1);


	private:
		Ui::sw *ui;
		void savesettings(QString key, int value);
		void savesettings(QString key, QString value);
		QString loadsettings(QString key, QString defaultval);
		void closeEvent (QCloseEvent *event);

};

#endif // SW_H
