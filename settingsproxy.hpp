#ifndef SETTINGSPROXY_HPP
#define SETTINGSPROXY_HPP

#include <QObject>
#include <QSettings>
#include <QStringList>

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

    QStringList getRecentFiles();
    QString getMostRecentFile();
    void resetRecentFiles();

    // Add the given filename to the recent files list. That is:
    //   0. If the file is on the list and at position 0, don't do anything
    //   1. If the file is not on the list, add it to front and move everything else back
    //   2. If the file is already there on the list, remove it and then do outcome 1,
    //      essentially moving it to front
    // Returns true if any change happened at all (outcome 1 and 2).
    bool addToRecentFiles(QString filename);

protected:
    QSettings settings;
    unsigned short recentCount;
    StartupAction startupAction;
};



#endif // SETTINGSPROXY_HPP
