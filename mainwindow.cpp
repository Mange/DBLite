#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "settingswindow.hpp"

MainWindow::MainWindow(QString filename, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    init();
    if (!filename.isEmpty())
        openFiles(filename);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    init();
}

MainWindow::~MainWindow()
{
}

/* Methods */

void MainWindow::init()
{
    ui->setupUi(this);
    highlighter = new SqlHighlighter(ui->queryEdit->document());

    setAttribute(Qt::WA_DeleteOnClose);
    setUnifiedTitleAndToolBarOnMac(true);

    loadSettings();

    // Connect slots
    connect(this, SIGNAL(fileOpened()), this, SLOT(updateTitle()));
    connect(this, SIGNAL(fileOpened()), this, SLOT(setActionStates()));
    connect(this, SIGNAL(fileOpened()), this, SLOT(reloadTableTree()));

    connect(this, SIGNAL(fileOpened()), this, SLOT(pushToMruList()));

    connect(ui->actionReloadTree, SIGNAL(triggered()), this, SLOT(reloadTableTree()));
    connect(ui->actionNewWindow,  SIGNAL(triggered()), this, SLOT(openNewWindow()));

    updateTitle();
    setActionStates();
    refreshMruList();
}

void MainWindow::loadSettings()
{
    settings.loadSettings();
}

void MainWindow::createMruActions(unsigned short count)
{
    if (mruActions.size() > count)
    {
        while(mruActions.size() > count)
            delete mruActions.takeLast();
    }

    for (unsigned short i = mruActions.size(); i < count; i++)
    {
        mruActions << new QAction(this);
        mruActions[i]->setVisible(false);
        ui->menuOpenRecent->addAction(mruActions[i]);
        connect(mruActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }
}

QString MainWindow::askForFilename()
{
    return QFileDialog::getOpenFileName(this, tr("Open database file"));
}

QStringList MainWindow::askForFilenames()
{
    return QFileDialog::getOpenFileNames(this, tr("Open database files"));
}

bool MainWindow::loadFile(QString filename)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    openedFile.open(filename);
    QApplication::restoreOverrideCursor();

    if(openedFile.valid())
    {
        emit fileOpened();
        return true;
    }
    else
    {
        QMessageBox::warning(
                this,
                tr("Unable to open database"),
                tr("An error occurred while opening the database file: %1").
                    arg(openedFile.lastError())
        );
        return false;
    }
}

void MainWindow::openFiles(QString filename)
{
    if (filename.isEmpty())
        return;

    if (openedFile.valid())
        openNewWindow(filename);
    else
        loadFile(filename);
}

void MainWindow::openFiles(QStringList filenames)
{
    foreach (QString filename, filenames)
        openFiles(filename);
}

void MainWindow::setStatusBarMessage(QString message)
{
    this->statusBar()->showMessage(message);
}

void MainWindow::resetResultView()
{
    delete ui->resultView->model();
}

/* Slots */

void MainWindow::updateTitle()
{
    if (openedFile.valid())
        this->setWindowTitle(tr("DBLite - %1").arg(openedFile.fileName()));
    else
        this->setWindowTitle(tr("DBLite"));
}

void MainWindow::setActionStates()
{
    bool opened = openedFile.valid();

    ui->actionExecute_query->setEnabled(opened);
    ui->actionReloadTree->setEnabled(opened);
}

void MainWindow::openRecentFile()
{
    QAction *senderAction = qobject_cast<QAction*>(sender());
    if (senderAction)
        openFiles(senderAction->data().toString());
}

void MainWindow::openNewWindow(QString filename)
{
    MainWindow *other = new MainWindow(filename);
    other->move(this->x() + 20, this->y() + 20);
    other->show();
}

void MainWindow::pushToMruList()
{
    settings.addToRecentFiles(openedFile.fullFileName());
    globalRefreshMruList();
}

void MainWindow::refreshMruList()
{
    loadSettings();
    QStringList files = settings.getSettings()->value("Recent files").toStringList();

    // Make sure we don't iterate too far
    unsigned short actionsCount = qMin(files.size(), (int) settings.getRecentCount());

    // Ensure right amount of actions
    createMruActions(actionsCount);

    // Update the data, titles, etc. for all the actions
    for (unsigned short i = 0; i < actionsCount; i++)
    {
        // Most Recently Used action text. Argument 1 is the number, 2 is the filename
        QString actionText = tr("&%1: %2").
                             arg(QString::number(i+1)).
                             arg(QFileInfo(files[i]).fileName());

        mruActions[i]->setData(files[i]);
        mruActions[i]->setText(actionText);
        mruActions[i]->setVisible(true);
    }

    // Hide the rest
    for (unsigned short i = actionsCount; i < mruActions.size(); i++)
    {
        mruActions[i]->setVisible(false);
    }

    // Enable the submenu if we had any files visible
    ui->menuOpenRecent->setEnabled(actionsCount > 0);
}

void MainWindow::globalRefreshMruList()
{
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
         MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
         if (mainWin)
             mainWin->refreshMruList();
     }
}

void MainWindow::reloadTableTree()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    ui->tableTree->clear();
    if (!openedFile.valid())
        return;

    // Set the root node to the opened database
    QTreeWidgetItem *root = new QTreeWidgetItem(ui->tableTree);
    root->setText(0, openedFile.fileName());
    root->setIcon(0, QIcon(":/main/icons/document-database.png"));

    // Get all tables and add them in order
    QStringList tables = openedFile.getTables();
    foreach (QString tableName, tables)
    {
        int rowCount = openedFile.getRowCount(tableName);

        QTreeWidgetItem *table = new QTreeWidgetItem(root);
        table->setText(0, tableName);
        table->setText(1, QString::number(rowCount));
        table->setIcon(0, QIcon(":/main/icons/table.png"));

        // Add all fields for the table
        QList<DbField> fields = openedFile.getFields(tableName);
        foreach(DbField fieldInfo, fields)
        {
            QTreeWidgetItem *field = new QTreeWidgetItem(table);
            field->setText(0, fieldInfo.first);
            field->setText(1, fieldInfo.second);
            field->setIcon(0, QIcon(":/main/icons/field.png"));
        }
    }

    // Pre-expand root node
    root->setExpanded(true);

    QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::closeAllWindows();
}

void MainWindow::on_actionOpen_triggered()
{
    openFiles(askForFilenames());
}

void MainWindow::on_actionExecute_query_triggered()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    resetResultView();
    QSqlQueryModel *model = new QSqlQueryModel(ui->resultView);
    openedFile.executeQuery(ui->queryEdit->toPlainText(), model);
    ui->resultView->setModel(model);

    if (model->lastError().type() != QSqlError::NoError)
    {
        QMessageBox::warning(
                this,
                tr("Error while executing query"),
                tr("There was an error in your query. Error was:\n%1: %2").
                        arg(model->lastError().number()).
                        arg(model->lastError().databaseText())
        );
        return;
    }

    QString message;
    if (model->query().isSelect())
    {
        // If QSQLITE would have feature QSqlDriver::QuerySize, we could use
        //    model->query().size()
        // but for now, we'll have to settle for this
        message = tr("Query returned %n row(s)", "", model->rowCount());
    }
    else
    {
        message = tr("Query affected %n row(s)", "", model->query().numRowsAffected());
    }
    setStatusBarMessage(message);

    QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionSettings_triggered()
{
    // Open the settings window
    SettingsWindow settings(this);
    settings.setWindowModality(Qt::ApplicationModal);
    settings.exec();

    // Reload all settings
    loadSettings();
    globalRefreshMruList();
}
