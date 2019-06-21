#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <QLayout>

namespace  util
{

	void sleep(int duration);
	void sleepSeconds(int duration);


	QString toCamelCase(const QString& s);
	QString qnow();
	void clearLayout(QLayout* layout);

	struct success_message{
			bool result;
			QString message;
			operator bool() {
				return result;
			}
			bool is_error(){
				return !result;
			}
			success_message(bool s, QString m){
				result = s; message=m;
			}
	};

};

#endif // UTIL_H
