#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>

class SettingsManager
{
private:
	SettingsManager();

public:
	static SettingsManager *instance();
	QString getLastUpdateRep();
	void setLastUpdateRep(QString date);

private:
	QSettings	*m_settings;
	QString		m_lastUpdateRepKey = "lastUpdateRep";
};

#endif // SETTINGSMANAGER_H
