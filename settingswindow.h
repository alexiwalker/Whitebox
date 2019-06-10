#ifndef SW_H
#define SW_H

#include <QMainWindow>

namespace Ui {
	class sw;
}

class settingsWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit settingsWindow(QWidget *parent = nullptr);
		~settingsWindow();

	private slots:
		void on_librarybutton_clicked();

		void on_sourcebutton_clicked();

		void on_autoindex_stateChanged(int arg1);


	private:
		Ui::sw *ui;
		void closeEvent (QCloseEvent *event);

};

#endif // SW_H
