#include "settingswindow.hpp"
#include "ui_settingswindow.h"

SettingsProxy::SettingsProxy()
{
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
    QSettings settings("Magnus Bergmark", "DBLite");
    recentCount = settings.value("Max recent files", 5).toInt();
    startupAction = startupActionCast(settings.value("Startup action", (int) BlankWindow).toInt());
}

void SettingsProxy::saveSettings()
{
    QSettings settings("Magnus Bergmark", "DBLite");
    settings.setValue("Max recent files", recentCount);
    settings.setValue("Startup action", (int) startupAction);
}

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    proxy.loadSettings();
    applyProxyToForm();
    // TODO: On Mac, hide the buttons at the bottom and make the settings save instantly
}

void SettingsWindow::applyProxyToForm()
{
    ui->spinMruCount->setValue(proxy.recentCount);

    ui->radioStartupBlank   ->setChecked(proxy.startupAction == BlankWindow);
    ui->radioStartupLastFile->setChecked(proxy.startupAction == LastOpenedFile);
}

void SettingsWindow::on_buttonBox_clicked(QAbstractButton *activated)
{
    QDialogButtonBox::ButtonRole role = ui->buttonBox->buttonRole(activated);
    if (role == QDialogButtonBox::AcceptRole || role == QDialogButtonBox::ApplyRole)
        proxy.saveSettings();
}

void SettingsWindow::on_radioStartupBlank_toggled(bool checked)
{
    if (checked)
        proxy.startupAction = BlankWindow;
}

void SettingsWindow::on_radioStartupLastFile_toggled(bool checked)
{
    if (checked)
        proxy.startupAction = LastOpenedFile;
}

void SettingsWindow::on_spinMruCount_valueChanged(int count)
{
    proxy.recentCount = count;
}
