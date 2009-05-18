#include "settingsproxy.hpp"

SettingsProxy::SettingsProxy()
    : settings("Magnus Bergmark", "DBLite")
{
}

QSettings *SettingsProxy::getSettings()
{
    return &settings;
}

StartupAction SettingsProxy::startupActionCast(int action)
{
    if (action >= BlankWindow && action < StartupActionCount)
    {
        return (StartupAction) action;
    }
    return BlankWindow;
}

void SettingsProxy::loadSettings()
{
    recentCount = settings.value("Max recent files", 5).toInt();
    startupAction = startupActionCast(settings.value("Startup action", (int) BlankWindow).toInt());
}

void SettingsProxy::saveSettings()
{
    settings.setValue("Max recent files", recentCount);
    settings.setValue("Startup action", (int) startupAction);
}

StartupAction SettingsProxy::getStartupAction()
{
    return startupAction;
}

void SettingsProxy::setStartupAction(StartupAction action)
{
    startupAction = action;
}

void SettingsProxy::setStartupAction(int action)
{
    setStartupAction(startupActionCast(action));
}

unsigned short SettingsProxy::getRecentCount()
{
    return recentCount;
}

void SettingsProxy::setRecentCount(unsigned short count)
{
    recentCount = count;
}
