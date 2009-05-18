#include "settingswindow.hpp"
#include "ui_settingswindow.h"

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
    ui->spinMruCount->setValue(proxy.getRecentCount());

    ui->radioStartupBlank   ->setChecked(proxy.getStartupAction() == BlankWindow);
    ui->radioStartupLastFile->setChecked(proxy.getStartupAction() == LastOpenedFile);
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
        proxy.setStartupAction(BlankWindow);
}

void SettingsWindow::on_radioStartupLastFile_toggled(bool checked)
{
    if (checked)
        proxy.setStartupAction(LastOpenedFile);
}

void SettingsWindow::on_spinMruCount_valueChanged(int count)
{
    proxy.setRecentCount(count);
}

void SettingsWindow::on_buttonResetMru_clicked()
{
    proxy.getSettings()->setValue("Recent files", QStringList());
}
