#ifndef SETTINGSWINDOW_HPP
#define SETTINGSWINDOW_HPP

#include <QtGui/QDialog>
#include <QtGui>

#include "settingsproxy.hpp"

namespace Ui
{
    class SettingsWindow;
}

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
    void on_buttonResetMru_clicked();
    void on_spinMruCount_valueChanged(int count);
    void on_radioStartupLastFile_toggled(bool checked);
    void on_radioStartupBlank_toggled(bool checked);
    void on_buttonBox_clicked(QAbstractButton *activated);
};

#endif // SETTINGSWINDOW_HPP
