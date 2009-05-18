#ifndef SETTINGSPROXY_HPP
#define SETTINGSPROXY_HPP

#include <QObject>
#include <QSettings>

enum StartupAction {
    BlankWindow,
    LastOpenedFile,
    StartupActionCount
};

/* Define a settings proxy object to save the settings in before saving to permanent storage */
class SettingsProxy : public QObject
{
    Q_OBJECT;

public:
    SettingsProxy();

    QSettings *getSettings();
    StartupAction startupActionCast(int action);
    void loadSettings();
    void saveSettings();

    StartupAction getStartupAction();
    void setStartupAction(StartupAction action);
    void setStartupAction(int action);

    unsigned short getRecentCount();
    void setRecentCount(unsigned short count);

protected:
    QSettings settings;
    unsigned short recentCount;
    StartupAction startupAction;
};



#endif // SETTINGSPROXY_HPP
