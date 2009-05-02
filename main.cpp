#include <QtGui/QApplication>
#include "mainwindow.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow *w = new MainWindow();
    if (w->valid())
    {
        w->show();
        return a.exec();
    }
    else
    {
        exit(0);
    }
}
