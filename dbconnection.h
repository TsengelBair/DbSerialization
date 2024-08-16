#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

class DbConnection
{
private:

    QSqlDatabase db;
    static DbConnection* instance;

    DbConnection() {
        db = QSqlDatabase::addDatabase("QPSQL");
        db.setUserName("postgres");
        db.setHostName("localhost");
        db.setPassword("25944");
        openDatabase();
    }

    ~DbConnection() {
        closeDatabase();
    }

    void openDatabase() {
        if (db.open()) {
            qDebug() << "Успешное подключение к базе данных";
        } else {
            qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
        }
    }

    void closeDatabase() {
        if (db.isOpen()) {
            db.close();
        }
    }

public:
    static DbConnection* getInstance() {
        if (instance == nullptr) {
            instance = new DbConnection();
        }
        return instance;
    }

    QSqlDatabase& getDatabase() {
        return db;
    }

    // Метод для изменения бд
    void setDatabaseConfig(const QString& dbName) {

        closeDatabase();
        db.setDatabaseName(dbName);
        openDatabase();
    }
};

#endif // DBCONNECTION_H
