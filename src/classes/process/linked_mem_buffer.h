#ifndef LINKED_MEM_BUFFER_H
#define LINKED_MEM_BUFFER_H

#include <QSharedMemory>
#include <QString>
#include <QBuffer>
#include <QDataStream>


class SharedMemory
	{
public:
	SharedMemory(QString key,bool create);

	QString read();
	void write(QString string);
	void write(char* string);
	void clear();
	bool exists();

	/* for specifying usages with arbitrary values, add/remove/modify as needed */
	static QString SINGLE_INSTANCE;
	static QString FILE_HANDLE;

	bool attach();

	bool create();

	void close();

private:
	QSharedMemory* sharedMemory;

	};

#endif // LINKED_MEM_BUFFER_H
