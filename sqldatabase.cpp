#include "sqldatabase.h"

sqlDatabase::sqlDatabase()
{

}

sqlDatabase::~sqlDatabase()
{
    m_db.close();
    m_db.removeDatabase(QSqlDatabase::defaultConnection);
}

bool sqlDatabase::openDb()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("ausgaben.db");

    if(!m_db.open()){
        qDebug() << ("db fail");
        return false;
    }else{
        qDebug() << ("db open!");
        return true;
    }
}

QSqlQuery *sqlDatabase::executeQuery(QString command)
{
    QSqlQuery* query = new QSqlQuery(m_db);

    (query->prepare(command) == true) ? qDebug() << "prepare succeed" :
                                        qDebug() << "prepare failed";
    query->exec();

    return query;
}
