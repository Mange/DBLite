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
    MainWindow *w;

    // Look for command line arguments
    // FIXME: I really, really don't like the code below. I should probably find a good way to refactor it.
    QStringList arguments = a.arguments();
    if (arguments.size() > 1)
    {
        w = new MainWindow(arguments[1]);
    }
    else
    {
        settings->loadSettings();
        if (settings->getStartupAction() == LastOpenedFile && !settings->getMostRecentFile().isEmpty())
            w = new MainWindow(settings->getMostRecentFile());
        else
            w = new MainWindow();
    }

    delete settings;
    w->show();
    return a.exec();
}
