#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QtSql/QtSql>

class sqlDatabase
{
public:
    sqlDatabase();
    ~sqlDatabase();

    bool openDb();
    QSqlQuery* executeQuery(QString command);

private:
    QSqlDatabase m_db;

};

#endif // SQLDATABASE_H
