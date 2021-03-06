#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtGui/QMainWindow>

#include <QtGui>
#include <QtSql>

#include "sqlhighlighter.hpp"
#include "databasefile.hpp"
#include "settingsproxy.hpp"

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
    MainWindow(QString filename, QWidget *parent = 0);
    ~MainWindow();

private:
    /* Members */
    Ui::MainWindow *ui;
    DatabaseFile openedFile;
    SqlHighlighter *highlighter;
    SettingsProxy settings;

    QList<QAction*> mruActions;

    /* Methods */

    // Helper method to keep common constructor logic in one place
    void init();
    void loadSettings();

    // Make sure we have count MRU actions present. This might mean deleting and it might mean creating new ones
    void createMruActions(unsigned short count);

    // Returns a QString to a path chosen by the user. If the user aborted, the null string will be returned
    QString askForFilename();

    // Returns a QStringList of n chosen files by the user. If the user aborted, the list will be empty
    QStringList askForFilenames();

    // Opens and load the given file in the current window. On error, false will be returned.
    // On success, fileOpened signal will be emitted and true returned.
    bool loadFile(QString filename);

    // Opens the given filename, in this window if nothing is opened here, and in another window if
    // a file already is open in this one.
    void openFiles(QString filename);

    // Opens the given files. Practicly, the filenames will be iterated and opened with the single
    // file version of this method.
    void openFiles(QStringList filenames);

    // Helper method for setting message in status bar
    void setStatusBarMessage(QString message);

    // Resets (clears) the result view. Not undoable, and not just cosmetic.
    void resetResultView();

signals:
    // Emitted when a file is opened in the current window
    void fileOpened();

    // TODO: Add a signal for query about the be executed (before)
    // TODO: Add a signal for queries doing changes to the database (after)
    // TODO: Add a signal for queries doing selects from the database (after)
    // TODO: Add a signal for queries executed (after)
    // TODO: Add a signal for resultView resetted
    // TODO: Add a signal for resultView updated

private slots:
    /* Internal slots */

    // Updates the title to contain the opened database's name
    void on_actionSettings_triggered();
    void updateTitle();

    // Enables / Disables states depending on a file is opened or not
    void setActionStates();

    // Open a recently opened file. The file itself will be derived from the sender action's data
    void openRecentFile();

    // Open a new window with the given file name loaded.
    void openNewWindow(QString filename = QString());

    // Updates recently used list. The currently opened file will be appended to the list,
    // and the setting saved
    void pushToMruList();

    // Refreshes the recently used list in the UI. Only updates all the actions in the menu
    // from the settings, nothing more.
    void refreshMruList();

    // Refreshes the MRU list in every opened window
    void globalRefreshMruList();

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
