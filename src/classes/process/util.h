#ifndef UTIL_H
#define UTIL_H
#include <QString>

namespace  util
	{

	void sleep(int duration);
	void sleepSeconds(int duration);


	QString toCamelCase(const QString& s);
	QString qnow();


	};

#endif // UTIL_H
