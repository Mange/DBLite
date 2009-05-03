#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Check if we have any DB drivers ready
    if( !QSqlDatabase::drivers().contains("QSQLITE", Qt::CaseInsensitive) ) {
        QMessageBox::critical(
                this,
                tr("SQLite database driver not found"),
                tr("Could not find the Qt database driver QSQLITE in the system. You need"
                   "this source to connect to SQLite databases.")
        );
        exit(1);
    }

    init();
}

MainWindow::~MainWindow()
{
}

bool MainWindow::valid()
{
    return dbPath != QString();
}

void MainWindow::init()
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    setUnifiedTitleAndToolBarOnMac(true);
    dbIdentifier = QString();

    // Connect slots
    connect(this, SIGNAL(openedStatusChanged(bool)), this, SLOT(updateTitle()));
    connect(this, SIGNAL(openedStatusChanged(bool)), this, SLOT(setActionStates(bool)));
    connect(this, SIGNAL(openedStatusChanged(bool)), this, SLOT(reloadTableTree()));

    connect(ui->actionReloadTree, SIGNAL(triggered()), this, SLOT(reloadTableTree()));

    emit initialized();
}

void MainWindow::initialized()
{
    if(!openFile())
    {
        emit close();
    }
    else
    {
        emit openedStatusChanged(true);
    }
}

bool MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open database"));
    if (path != QString())
    {
        return openFile(path);
    }
    else
    {
        return false;
    }
}

bool MainWindow::openFile(QString path)
{
    dbPath = path;
    dbName = QFileInfo(dbPath).fileName();
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
                tr("An error occurred while opening the connection: ") + db.lastError().text()
                );
        dbName = QString();
        dbPath = QString();
        dbIdentifier = QString();
        return false;
    }
    else
    {
        return true;
    }
}

void MainWindow::resetResultView()
{
    delete ui->resultView->model();
}

void MainWindow::setActionStates(bool opened)
{
    ui->actionExecute_query->setEnabled(opened);
    ui->executeQueryButton->setEnabled(opened);
    ui->actionReloadTree->setEnabled(opened);
}

void MainWindow::updateTitle()
{
    this->setWindowTitle(tr("DBLite - %1").arg(dbName));
}

QString MainWindow::getDatabaseType(QSqlField field)
{
    switch(field.type())
    {
        case QVariant::String:
            return tr("String");
        case QVariant::Int:
            return tr("Integer");
        case QVariant::Double:
            return tr("Real");
        default:
            return tr("Unknown type: %1").arg(field.type());
    }
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
        }
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
    else
    {
        return -1;
    }
}

void MainWindow::reloadTableTree()
{
    ui->tableTree->clear();
    if (dbName == QString()) { return; }
    
    // Set the root node to the opened database
    QTreeWidgetItem *root = new QTreeWidgetItem(ui->tableTree, QStringList(dbName), 0);

    // Get all tables and add them in order
    QStringList tables = QSqlDatabase::database(dbIdentifier, true).tables(QSql::AllTables);
    QStringList::const_iterator constIterator;
    for(constIterator = tables.begin(); constIterator != tables.end(); constIterator++)
    {
        QString tableName = (*constIterator);

        // Give a random row count for now
        QTreeWidgetItem *table = new QTreeWidgetItem(root);
        table->setText(0, tableName);
        table->setText(1, QString::number(getRowCount(tableName, dbIdentifier)));

        // Load the description for the table
        loadTableDescription(tableName, dbIdentifier, table);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    MainWindow *other = new MainWindow();
    if (other->valid())
    {
        other->move(this->x() + 10, this->y() + 10);
        other->show();
    }
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
    }
    else if (model->query().isSelect())
    {
        emit setStatusTip(tr("Fetched %1 rows").arg(model->query().size()));
    }
    else
    {
        emit setStatusTip(tr("Affected %1 rows").arg(model->query().numRowsAffected()));
    }
}

void MainWindow::on_actionQuit_triggered()
{
    // Close everything and quit
    ui->actionClose->trigger();
    emit close();
}
