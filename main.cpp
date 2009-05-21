#include <QtGui/QApplication>
#include "mainwindow.hpp"
#include "settingsproxy.hpp"

int main(int argc, char *argv[])
{    
    QApplication a(argc, argv);
    a.setApplicationName("DBLite");

/* TODO: Make this work like it should. Seems to be pretty hard to do and the manual doesn't want to touch it */
#ifdef Q_WS_MAC_TODO
    // Mac specific code to handle the separation of application and window

    // First of all, don't quit just because we don't have any windows
    a.setQuitOnLastWindowClosed(false);

    // Second, create a root menu bar for when all the windows is closed
    QMenuBar *rootMenu = new QMenuBar();
    QAction quitAction("Quit", rootMenu);
    quitAction.connect(quitAction, SIGNAL(triggered()), a, SLOT(quit()));

    rootMenu->addAction(quitAction);
#endif

    SettingsProxy *settings = new SettingsProxy();
    settings->loadSettings();

    MainWindow *w;

    // Look for command line arguments
    QString fileToOpen;
    QStringList arguments = a.arguments();
    if (arguments.size() > 1)
        fileToOpen = arguments.at(0);
    else if (settings->getStartupAction() == LastOpenedFile && !settings->getMostRecentFile().isEmpty())
        fileToOpen = settings->getMostRecentFile();
    else
        fileToOpen = QString();

    delete settings;

    w = new MainWindow(fileToOpen);
    w->show();
    return a.exec();
}
