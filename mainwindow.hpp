#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtGui/QMainWindow>

#include <QtGui>
#include <QtSql>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

// TODO: Implement proxy icon per http://doc.trolltech.com/qq/qq18-macfeatures.html

public:
    MainWindow(QWidget *parent = 0);
    MainWindow(QString fileName, QWidget *parent);
    ~MainWindow();

    // Returns true when a file has been opened. Use this before displaying a new window to
    // know if the user actually opened a file or if the window is about to autoclose.
    bool valid();

private:
    void init();
    bool openFile();
    bool openFile(QString fileName);
    void setStatusBarMessage(QString message);
    void resetResultView();
    void loadTableDescription(QString tableName, QString dbIdentifier, QTreeWidgetItem *parent);
    QString getDatabaseType(QSqlField field);
    int getRowCount(QString tableName, QString dbIdentifier);

    Ui::MainWindow *ui;

    QString dbIdentifier;
    QString dbName;
    QString dbPath;

signals:
    void openedStatusChanged(bool);

private slots:
    void initialized();
    void on_actionQuit_triggered();
    void on_actionExecute_query_triggered();
    void on_actionOpen_triggered();
    void updateTitle();
    void setActionStates(bool);
    void reloadTableTree();
};

#endif // MAINWINDOW_HPP
