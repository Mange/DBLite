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
    ~MainWindow();

private:
    void resetResultView();
    void loadTableDescription(QString tableName, QString dbIdentifier, QTreeWidgetItem *parent);
    QString getDatabaseType(QSqlField field);
    int getRowCount(QString tableName, QString dbIdentifier);

    Ui::MainWindow *ui;
    const char *dbIdentifier;

    QString dbName;
    QString dbPath;

signals:
    void openedStatusChanged(bool);

private slots:
    void on_actionQuit_triggered();
    void on_actionClose_triggered();
    void on_actionExecute_query_triggered();
    void on_actionOpen_triggered();
    void updateTitle(bool);
    void setActionStates(bool);
    void reloadTableTree();
};

#endif // MAINWINDOW_HPP
