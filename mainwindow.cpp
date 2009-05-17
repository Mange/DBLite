#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString fileName, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    init();

    // Open the given file
    if(!openFile(fileName))
        emit close();
    else
        emit openedStatusChanged(true);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    init();

    // Prompt the user for a file
    if(!openFile())
        emit close();
    else
        emit openedStatusChanged(true);
}

MainWindow::~MainWindow()
{
}

/* Methods */

bool MainWindow::openNewWindow(QString filename)
{
    MainWindow *other;
    if (filename == QString())
        other = new MainWindow();
    else
        other = new MainWindow(filename);

    if (other->valid())
    {
        other->move(this->x() + 10, this->y() + 10);
        other->show();
    }
}

bool MainWindow::valid()
{
    return dbPath != QString();
}

void MainWindow::init()
{
    ui->setupUi(this);
    highlighter = new SqlHighlighter(ui->queryEdit->document());

    setAttribute(Qt::WA_DeleteOnClose);
    setUnifiedTitleAndToolBarOnMac(true);
    dbIdentifier = QString();
    maxMruItems = 5;

    // Create mru actions
    //mruActions = QList<QAction*>();
    for (unsigned short i = 0; i < maxMruItems; i++)
    {
        mruActions << new QAction(this);
        mruActions[i]->setVisible(false);
        ui->menuOpenRecent->addAction(mruActions[i]);
        connect(mruActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

    // Connect slots
    connect(this, SIGNAL(openedStatusChanged(bool)), this, SLOT(updateTitle()));
    connect(this, SIGNAL(openedStatusChanged(bool)), this, SLOT(setActionStates(bool)));
    connect(this, SIGNAL(openedStatusChanged(bool)), this, SLOT(reloadTableTree()));

    // TODO: Fix a better signal :-(
    connect(this, SIGNAL(openedStatusChanged(bool)), this, SLOT(pushToMruList()));
    connect(this, SIGNAL(mruChanged()), this, SLOT(refreshMruList()));

    connect(ui->actionReloadTree, SIGNAL(triggered()), this, SLOT(reloadTableTree()));

    emit initialized();
}

bool MainWindow::openFile(bool newWindow)
{
    if (newWindow)
        return openNewWindow();

    QString path = QFileDialog::getOpenFileName(this, tr("Open database"));
    if (path != QString())
        return openFile(path);
    return false;
}

bool MainWindow::openFile(QString path, bool newWindow)
{
    if (newWindow)
        return openNewWindow(path);

    dbPath       = path;
    dbName       = QFileInfo(dbPath).fileName();
    dbIdentifier = QString("opened_db_%1").arg(dbName);

    // Check if we already have the connection opened and reuse in that case
    QSqlDatabase db;
    if (QSqlDatabase::connectionNames().contains(dbIdentifier))
    {
        db = QSqlDatabase::database(dbIdentifier);
    }
    else
    {
        db = QSqlDatabase::addDatabase("QSQLITE", dbIdentifier);
        db.setDatabaseName(dbPath);
    }

    if (!db.open())
    {
        QMessageBox::warning(
                this,
                tr("Unable to open database"),
                tr("An error occurred while opening the connection: %1").arg(db.lastError().text())
        );
        dbName       = QString();
        dbPath       = QString();
        dbIdentifier = QString();
        return false;
    }
    return true;
}

void MainWindow::setStatusBarMessage(QString message)
{
    this->statusBar()->showMessage(message);
}

void MainWindow::resetResultView()
{
    delete ui->resultView->model();
}

void MainWindow::loadTableDescription(QString tableName, QString dbIdentifier, QTreeWidgetItem *parent)
{
    QSqlRecord record = QSqlDatabase::database(dbIdentifier).record(tableName);
    if (record.count() > 0)
    {
        for(int i = 0; i < record.count(); i++)
        {
            QSqlField field = record.field(i);

            QTreeWidgetItem *column = new QTreeWidgetItem(parent);
            column->setText(0, field.name());
            column->setText(1, getDatabaseType(field));
            column->setIcon(0, QIcon(":/main/icons/field.png"));
        }
    }
}

QString MainWindow::getDatabaseType(QSqlField field)
{
    switch(field.type())
    {
        case QVariant::String:
            // Field type
            return tr("String");
        case QVariant::Int:
            // Field type
            return tr("Integer");
        case QVariant::Double:
            // Field type
            return tr("Real");
        default:
            // Field type
            return tr("Unknown type: %1").arg(field.type());
    }
}

int MainWindow::getRowCount(QString tableName, QString dbIdentifier)
{
    QSqlQuery query(QSqlDatabase::database(dbIdentifier));
    if (query.exec(QString("SELECT COUNT(*) FROM %1").arg(tableName)))
    {
        query.first();
        int count = query.value(0).toInt();
        query.finish();
        return count;
    }
    return -1;
}

/* Slots */

void MainWindow::initialized()
{
    // Check if we have any DB drivers ready
    if( !QSqlDatabase::drivers().contains("QSQLITE", Qt::CaseInsensitive) )
    {
        QMessageBox::critical(
                this,
                tr("SQLite database driver not found"),
                tr("Could not find the Qt database driver QSQLITE in the system. You need"
                   "this source to connect to SQLite databases.")
        );
        exit(1);
    }
}

void MainWindow::updateTitle()
{
    // Title of the main window. Database name will be expanded at position 1
    this->setWindowTitle(tr("DBLite - %1").arg(dbName));
}

void MainWindow::setActionStates(bool opened)
{
    ui->actionExecute_query->setEnabled(opened);
    ui->actionReloadTree->setEnabled(opened);
}

bool MainWindow::openRecentFile()
{
    QAction *senderAction = qobject_cast<QAction*>(sender());
    if (senderAction)
        return openFile(senderAction->data().toString(), true);
    return false;
}

void MainWindow::pushToMruList()
{
    QSettings settings("Magnus Bergmark", "DBLite", this);
    QStringList files = settings.value("Recent files").toStringList();

    // FIXME: Just reorder when we push a path already there
    files.push_front(dbPath);
    while(files.size() > maxMruItems)
        files.removeLast();

    settings.setValue("Recent files", files);

    emit mruChanged();
}

void MainWindow::refreshMruList()
{
    QSettings settings("Magnus Bergmark", "DBLite", this);
    QStringList files = settings.value("Recent files").toStringList();

    // Make sure we don't iterate too far
    unsigned short actionsCount = qMin(files.size(), mruActions.size());

    // Update the data, titles, etc. for all the actions
    for (unsigned short i = 0; i < actionsCount; i++)
    {
        mruActions[i]->setData(files[i]);
        mruActions[i]->setText(QFileInfo(files[i]).fileName());
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

void MainWindow::reloadTableTree()
{
    ui->tableTree->clear();
    if (dbName == QString())
        return;

    // Set the root node to the opened database
    QTreeWidgetItem *root = new QTreeWidgetItem(ui->tableTree, QStringList(dbName), 0);
    root->setIcon(0, QIcon(":/main/icons/document-database.png"));

    // Get all tables and add them in order
    QStringList tables = QSqlDatabase::database(dbIdentifier, true).tables(QSql::AllTables);
    foreach (QString tableName, tables)
    {
        // Give a random row count for now
        QTreeWidgetItem *table = new QTreeWidgetItem(root);
        table->setText(0, tableName);
        table->setText(1, QString::number(getRowCount(tableName, dbIdentifier)));
        table->setIcon(0, QIcon(":/main/icons/table.png"));

        // Load the description for the table
        loadTableDescription(tableName, dbIdentifier, table);
    }
}

void MainWindow::on_actionQuit_triggered()
{
    // Close everything and quit
    // TODO: We should loop over all windows here, I guess.
    ui->actionClose->trigger();
    emit close();
}

void MainWindow::on_actionOpen_triggered()
{
    openNewWindow();
}

void MainWindow::on_actionExecute_query_triggered()
{
    resetResultView();
    QSqlQueryModel *model = new QSqlQueryModel(ui->resultView);
    model->setQuery(ui->queryEdit->toPlainText(), QSqlDatabase::database(dbIdentifier));
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

    int rows = 0;
    const char *message;
    if (model->query().isSelect())
    {
        // If QSQLITE would have feature QSqlDriver::QuerySize, we could use
        //    model->query().size()
        // but for now, we'll have to settle for this
        rows = model->rowCount();
        message = "Query returned %n row(s)";
    }
    else
    {
        rows = model->query().numRowsAffected();
        message = "Query affected %n row(s)";
    }
    setStatusBarMessage(tr(message, "", rows));
}
