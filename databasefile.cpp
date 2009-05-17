#include "databasefile.hpp"

DatabaseFile::DatabaseFile()
{
    m_opened = false;
}

DatabaseFile::DatabaseFile(QString filename)
{
    m_opened = false;
    open(filename);
}

QString DatabaseFile::getFieldKind(QSqlField field)
{
    // Comments below are for QtLinguist
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

void DatabaseFile::open(QString filename)
{
    m_filename = filename;

    // Reopen the db if already opened, else open it
    if (QSqlDatabase::connectionNames().contains(m_filename))
        m_db = QSqlDatabase::database(m_filename);
    else
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE", m_filename); // Identifier
        m_db.setDatabaseName(m_filename); // File to load. They're the same, but don't have to be
    }

    m_opened = true;
}

bool DatabaseFile::valid()
{
    return (m_opened && m_db.open());
}

QString DatabaseFile::lastError()
{
    return m_db.lastError().text();
}

QString DatabaseFile::fileName()
{
    return QFileInfo(m_filename).fileName();
}

QString DatabaseFile::fullFileName()
{
    return m_filename;
}

QStringList DatabaseFile::getTables()
{
    return m_db.tables(QSql::AllTables);
}

QList<DbField> DatabaseFile::getFields(QString tableName)
{
    QList<DbField> list;
    QSqlRecord record = m_db.record(tableName);

    if (record.count() > 0)
    {
        for(int i = 0; i < record.count(); i++)
        {
            QSqlField field = record.field(i);
            DbField fieldInfo;

            fieldInfo.first  = field.name();
            fieldInfo.second = getFieldKind(field);

            list << fieldInfo;
        }
    }

    return list;
}

int DatabaseFile::getRowCount(QString tableName)
{
    QSqlQuery query(m_db);
    if (query.exec(QString("SELECT COUNT(*) FROM %1").arg(tableName)))
    {
        query.first();
        int count = query.value(0).toInt();
        query.finish();
        return count;
    }
    return -1;
}

void DatabaseFile::executeQuery(QString query, QSqlQueryModel *model)
{
    model->setQuery(query, m_db);
}
