#include "util.h"
#include <thread>
#include <chrono>
#include <QString>
#include <QStringList>

void util::sleep(int duration)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}



void util::sleepSeconds(int duration)
{
	std::this_thread::sleep_for(std::chrono::seconds(duration));
}



QString util::toCamelCase(const QString& s){
	QStringList parts = s.split(' ', QString::SkipEmptyParts);
	for (int i = 0; i < parts.size(); ++i)
		parts[i].replace(0, 1, parts[i][0].toUpper());

	return parts.join(" ");
}

QString util::qnow(){
	auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	QString qnow = QString::number(now);

	return qnow;
}
