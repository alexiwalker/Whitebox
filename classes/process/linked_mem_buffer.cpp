#include "linked_mem_buffer.h"

QString linked_mem_buffer::SINGLE_INSTANCE  = "b345c1f06417fd9c";
QString linked_mem_buffer::FILE_HANDLE  = "5c53535bab1d2122";


linked_mem_buffer::linked_mem_buffer(QString key, bool create=false)
{
	sharedMemory = new QSharedMemory;
	sharedMemory->setKey(key);

	if(!exists() && create)
		sharedMemory->create(1);

}


QString linked_mem_buffer::read(){
	sharedMemory->lock();
	QString data = static_cast<char*>(sharedMemory->data());
	sharedMemory->unlock();
	return data;
}

void linked_mem_buffer::write(QString string){
	char* vals = string.toLocal8Bit().data();
	write(vals);
}

bool linked_mem_buffer::create(){
	bool b = sharedMemory->create(1);
	sharedMemory->attach();
	return b;
}

void linked_mem_buffer::write(char* string){
	QBuffer buffer;
	buffer.open(QIODevice::ReadWrite);
	buffer.write(string);
	int buffersize = buffer.size();

	//writes the string to a mem buffer, gets its size
	const char* from = buffer.data().data();

	sharedMemory->lock();

	char* to = static_cast<char*>(sharedMemory->data());

	//finds the data we're writing (from) and where we're putting it (to) (pointers)

	memcpy(to, from, qMin(sharedMemory->size(), buffersize));
	sharedMemory->unlock();

	//copt the data from the from location to the to location for a set duration

}

bool linked_mem_buffer::attach(){
	return sharedMemory->attach();
}

bool linked_mem_buffer::exists(){
	if(sharedMemory->isAttached())
		return true;
	else {
		if(sharedMemory->attach()){
			sharedMemory->detach();
			return true;
		}
	}

	return false;
}

void linked_mem_buffer::clear(){
	sharedMemory->lock();
	void* voidpt = sharedMemory->data();
	memset(voidpt,'\0', sharedMemory->size());
	sharedMemory->unlock();
}

