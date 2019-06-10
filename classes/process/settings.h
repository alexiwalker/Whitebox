#ifndef SETTINGS_H
#define SETTINGS_H
#include <QString>

class settings
{
	public:
		static QString set_app_details(QString app, QString org, QString mode="ini");
		static QString load_setting(QString key,QString _default);
		static void save_setting(QString key, QString value);
		static void save_setting(QString key, int value);
	private:
		static QString organizationName;
		static QString applicationName;
};

#endif // SETTINGS_H
