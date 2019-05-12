#include "wb.h"
#include <QApplication>
#include <QDebug>

#include <QSharedMemory>
#include <QString>
#include <QBuffer>
#include <QDataStream>
#include <classes/process/linked_mem_buffer.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	linked_mem_buffer shared_buffer(linked_mem_buffer::SINGLE_INSTANCE,false);
	if(shared_buffer.exists()){
		if(argc>1){
			shared_buffer.create();
			shared_buffer.write(argv[1]);
		}
		return 1;
	}

	shared_buffer.create();

	wb w;

	if(argc > 1)
		w.set_file_open_handle(a.arguments().at(1));

	w.show();

	return a.exec();
}
