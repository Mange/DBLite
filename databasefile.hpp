#ifndef DATABASEFILE_HPP
#define DATABASEFILE_HPP

#include <QObject>
#include <QtSql>

// Datatype for a table column - name and kind name in a pair
typedef QPair<QString,QString> DbField;

class DatabaseFile : public QObject
{
public:
    DatabaseFile();
    DatabaseFile(QString filename);

    static QString getFieldKind(QSqlField field);

    void open(QString filename);
    bool valid();
    QString lastError();

    QString fileName();
    QString fullFileName();

    // Information about the database
    QStringList getTables();
    QList<DbField> getFields(QString tableName);
    int getRowCount(QString tableName);

    void executeQuery(QString query, QSqlQueryModel *model);

private:
    QString m_filename;
    QSqlDatabase m_db;

    // We need this since QSqlDatabase doesn't say if we have a database opened or
    // not.
    bool m_opened;
};

#endif // DATABASEFILE_HPP
