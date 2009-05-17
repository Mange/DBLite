#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString filename, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    init();
    loadFile(filename);
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
    connect(this, SIGNAL(fileOpened()), this, SLOT(updateTitle()));
    connect(this, SIGNAL(fileOpened()), this, SLOT(setActionStates()));
    connect(this, SIGNAL(fileOpened()), this, SLOT(reloadTableTree()));

    connect(this, SIGNAL(fileOpened()), this, SLOT(pushToMruList()));
    connect(this, SIGNAL(mruChanged()), this, SLOT(refreshMruList()));

    connect(ui->actionReloadTree, SIGNAL(triggered()), this, SLOT(reloadTableTree()));

    updateTitle();
    setActionStates();
    refreshMruList();
}

void MainWindow::openNewWindow(QString filename)
{
    MainWindow *other = new MainWindow(filename);
    other->move(this->x() + 20, this->y() + 20);
    other->show();
}

QString MainWindow::askForFilename()
{
    return QFileDialog::getOpenFileName(this, tr("Open database file"));
}

bool MainWindow::loadFile(QString filename)
{
    openedFile.open(filename);
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

    ui->actionClose->setEnabled(opened);
    ui->actionExecute_query->setEnabled(opened);
    ui->actionReloadTree->setEnabled(opened);
}

void MainWindow::openRecentFile()
{
    QAction *senderAction = qobject_cast<QAction*>(sender());
    if (senderAction)
    {
        QString filename = senderAction->data().toString();
        if (openedFile.valid())
            openNewWindow(filename);
        else
            loadFile(filename);
    }
}

void MainWindow::pushToMruList()
{
    QSettings settings("Magnus Bergmark", "DBLite", this);
    QStringList files = settings.value("Recent files").toStringList();

    // FIXME: Just reorder when we push a path already there
    files.push_front(openedFile.fullFileName());
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
    if (openedFile.valid())
        openNewWindow(askForFilename());
    else
        loadFile(askForFilename());
}

void MainWindow::on_actionExecute_query_triggered()
{
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
}
