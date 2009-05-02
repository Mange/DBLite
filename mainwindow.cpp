#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dbIdentifier = "opened_db";
    dbName = QString();
    dbPath = QString();

    // Connect slots
    connect(this, SIGNAL(openedStatusChanged(bool)), this, SLOT(updateTitle(bool)));
    connect(this, SIGNAL(openedStatusChanged(bool)), this, SLOT(setActionStates(bool)));
    connect(this, SIGNAL(openedStatusChanged(bool)), this, SLOT(reloadTableTree()));

    connect(ui->actionReloadTree, SIGNAL(triggered()), this, SLOT(reloadTableTree()));


    // Check if we have any DB drivers ready
    if( QSqlDatabase::drivers().isEmpty() ) {
        QMessageBox::critical(this, tr("No database engines found!"), tr("No Qt Dabase plugins found. You need some drivers to connect to an source."));
        exit(1);
    }

    emit openedStatusChanged(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetResultView()
{
    delete ui->resultView->model();
}

void MainWindow::setActionStates(bool opened)
{
    ui->actionOpen->setEnabled(!opened);

    ui->actionClose->setEnabled(opened);
    ui->actionExecute_query->setEnabled(opened);
    ui->executeQueryButton->setEnabled(opened);
    ui->actionReloadTree->setEnabled(opened);
}

void MainWindow::updateTitle(bool opened)
{
    if (opened)
    {
        this->setWindowTitle(tr("DBLite - %1").arg(dbName));
    }
    else
    {
        this->setWindowTitle(tr("DBLite - No database opened"));
    }
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
    dbPath = QFileDialog::getOpenFileName(this, tr("Open database"));
    if (dbPath != QString())
    {
        dbName = QFileInfo(dbPath).fileName();

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", dbIdentifier);
        db.setDatabaseName(dbPath);
        if (!db.open())
        {
            QMessageBox::warning(
                    this,
                    tr("Unable to open database"),
                    tr("An error occurred while opening the connection: ") + db.lastError().text()
            );
            emit openedStatusChanged(false);
        }
        else
        {
            emit openedStatusChanged(true);
        }
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

void MainWindow::on_actionClose_triggered()
{
    QSqlDatabase::database(dbIdentifier, false).close();
    QSqlDatabase::removeDatabase(dbIdentifier);
    dbName = QString();
    dbPath = QString();

    resetResultView();
    emit openedStatusChanged(false);
}

void MainWindow::on_actionQuit_triggered()
{
    // Close everything and quit
    ui->actionClose->trigger();
    emit close();
}
