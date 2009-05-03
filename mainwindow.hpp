#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtGui/QMainWindow>

#include <QtGui>
#include <QtSql>

#include "sqlhighlighter.hpp"

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
    MainWindow(QString fileName, QWidget *parent = 0);
    ~MainWindow();

    // Returns true when a file has been opened. Use this before displaying a new window to
    // know if the user actually opened a file or if the window is about to autoclose.
    bool valid();

private:
    /* UI methods */

    // Helper method to keep common constructor logic in one place
    void init();

    // Prompts the user for a database file to open and then open it
    // TODO: Rename to something better
    bool openFile();

    // Open a given database
    bool openFile(QString fileName);

    // Helper method for setting message in status bar
    void setStatusBarMessage(QString message);

    // Resets (clears) the result view. Not undoable, and not just cosmetic.
    void resetResultView();

    // Load fields in a given table and populate the tree with them.
    // The table will be searched for via the given dbIdentifier and the
    // tree items will be added under the given parent item.
    //
    // Name derived from "DESCRIBE table" queries
    // TODO: Rename to something better.
    // TODO: Refactor. Separate UI creation with the querying
    void loadTableDescription(QString tableName, QString dbIdentifier, QTreeWidgetItem *parent);


    /* DB Methods */

    // Returns a human-readable string representing the given field's type
    QString getDatabaseType(QSqlField field);

    // Returns the number of rows in the given table located in the db connection identified by the given identifier
    int getRowCount(QString tableName, QString dbIdentifier);


    /* Members */

    Ui::MainWindow *ui;

    // Identifier used to identify the database connection. Must be
    // unique for every database, so the database name is appended.
    // TODO: Append a hash of the path instead; devel/main.db and production/main.db collides right now
    QString dbIdentifier;

    // Contains the full file path of the opened database. If this is
    // a null string, no database is open
    QString dbPath;

    // Contains the file name of the database
    QString dbName;

    SqlHighlighter *highlighter;

signals:
    // Emitted when a file is opened or closed. As good as deprecrated
    // by now.
    void openedStatusChanged(bool);

    // TODO: Add a signal for query about the be executed (before)
    // TODO: Add a signal for queries doing changes to the database (after)
    // TODO: Add a signal for queries doing selects from the database (after)
    // TODO: Add a signal for queries executed (after)
    // TODO: Add a signal for resultView resetted
    // TODO: Add a signal for resultView updated

private slots:
    /* Internal slots */

    // Do all after-constructor work here
    void initialized();

    // Updates the title to contain the opened database's name
    void updateTitle();

    // Update all the actions and their statuses.
    // As good as deprecrated by now.
    void setActionStates(bool);

    // Clears and readds information about the tables in the tree view
    // TODO: Make this a bit better by actually just updating instead of destroying/rebuilding
    void reloadTableTree();

    /* UI slots -- signals originating from the .ui file */
    // TODO: Add slot for tree item double-clicked
    void on_actionQuit_triggered();
    void on_actionOpen_triggered();
    void on_actionExecute_query_triggered();
};

#endif // MAINWINDOW_HPP
