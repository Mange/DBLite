#ifndef SETTINGSWINDOW_HPP
#define SETTINGSWINDOW_HPP

#include <QtGui/QDialog>
#include <QtGui>

namespace Ui
{
    class SettingsWindow;
}

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

    void loadSettings();
    void saveSettings();
    StartupAction startupActionCast(int action);

    unsigned short recentCount;
    StartupAction startupAction;
};

class SettingsWindow : public QDialog
{
    Q_OBJECT;

public:
    SettingsWindow(QWidget *parent = 0);

private:
    Ui::SettingsWindow *ui;
    SettingsProxy proxy;

    void applyProxyToForm();

private slots:
    void on_spinMruCount_valueChanged(int count);
    void on_radioStartupLastFile_toggled(bool checked);
    void on_radioStartupBlank_toggled(bool checked);
    void on_buttonBox_clicked(QAbstractButton *activated);
};

#endif // SETTINGSWINDOW_HPP
