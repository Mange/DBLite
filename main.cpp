#include <QtGui/QApplication>
#include "mainwindow.hpp"

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

    MainWindow *w;

    // Look for command line arguments
    QStringList arguments = a.arguments();
    if (arguments.size() > 1)
        w = new MainWindow(arguments[1]);
    else
        w = new MainWindow();

    // If the user tried to open a file and it failed (cancelled, etc.)
    // valid will be false and the window is about to destroy itself.
    // Don't try to display it in that case.
    if (w->valid())
    {
        w->show();
        return a.exec();
    }

    return 0;
}
