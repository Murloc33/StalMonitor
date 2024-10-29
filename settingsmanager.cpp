#include "settingsmanager.h"

SettingsManager::SettingsManager()
{
	m_settings = new QSettings("./StalMonitor.ini", QSettings::IniFormat);
}

SettingsManager *SettingsManager::instance()
{
	static SettingsManager manager;
	return &manager;
}

QString SettingsManager::getLastUpdateRep()
{
	return m_settings->value(m_lastUpdateRepKey).toString();
}

void SettingsManager::setLastUpdateRep(QString date)
{
	m_settings->setValue(m_lastUpdateRepKey, date);
}
