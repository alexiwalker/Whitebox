#include "settings.h"
#include <QCoreApplication>
#include <QString>
#include <QSettings>

QString settings::applicationName = "whitebox";
QString settings::organizationName = "Walkersoft";

void settings::save_setting(QString key, QString value) {
	QCoreApplication::setOrganizationName(organizationName);
	QCoreApplication::setApplicationName(applicationName);

	QSettings settings("settings.ini",QSettings::IniFormat);
	 settings.setValue(key, value);
}

void settings::save_setting(QString key, int value) {
	QCoreApplication::setOrganizationName(organizationName);
	QCoreApplication::setApplicationName(applicationName);


	QSettings settings("settings.ini",QSettings::IniFormat);
	settings.setValue(key, value);
}

QString settings::load_setting(QString key, QString defaultval) {
	QCoreApplication::setOrganizationName(organizationName);
	QCoreApplication::setApplicationName(applicationName);

	QSettings settings("settings.ini",QSettings::IniFormat);
	return settings.value(key, defaultval).toString();
}
