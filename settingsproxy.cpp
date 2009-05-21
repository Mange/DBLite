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

QStringList SettingsProxy::getRecentFiles()
{
    return settings.value("Recent files", QStringList()).toStringList();
}

QString SettingsProxy::getMostRecentFile()
{
    QStringList files = getRecentFiles();
    if (files.size() > 0)
        return files.at(0);
    else
        return QString();
}

void SettingsProxy::resetRecentFiles()
{
    settings.setValue("Recent files", QStringList());
}

bool SettingsProxy::addToRecentFiles(QString filename)
{
    QStringList currentFiles = getRecentFiles();

    // Exit early
    if (currentFiles.size() > 0 && currentFiles.at(0) == filename)
        return false;

    if (currentFiles.contains(filename))
        currentFiles.removeOne(filename);

    currentFiles.push_front(filename);
    while(currentFiles.size() > getRecentCount())
        currentFiles.removeLast();

    settings.setValue("Recent files", currentFiles);
    return true;
}
