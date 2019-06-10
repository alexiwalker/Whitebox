#ifndef LINKED_MEM_BUFFER_H
#define LINKED_MEM_BUFFER_H

#include <QSharedMemory>
#include <QString>
#include <QBuffer>
#include <QDataStream>


class linked_mem_buffer
	{
public:
	linked_mem_buffer(QString key,bool create);

	QString read();
	void write(QString string);
	void write(char* string);
	void clear();
	bool exists();

	/* Constants for specifying usages with arbitrary values */
	static QString SINGLE_INSTANCE;
	static QString FILE_HANDLE;

	bool attach();

	bool create();

	void close();

private:
	QSharedMemory* sharedMemory;

	};

#endif // LINKED_MEM_BUFFER_H
