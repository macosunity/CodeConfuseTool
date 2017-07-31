#ifndef DATABASE_H
#define DATABASE_H

#include <QTextCodec>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTime>
#include <QSqlError>
#include <QtDebug>
#include <QSqlDriver>
#include <QSqlRecord>
#include "classmodel.h"

#define DECLARE_SINGLETON(Class) \
Q_DISABLE_COPY(Class) \
public: \
    static Class* Instance() \
    { \
        static QMutex mutex; \
        static QScopedPointer<Class> inst; \
        if (Q_UNLIKELY(!inst)) { \
            mutex.lock(); \
            if (!inst) inst.reset(new Class); \
            mutex.unlock(); \
        } \
        return inst.data(); \
    }

class DataBase
{
    DECLARE_SINGLETON(DataBase);
private:
    bool m_isDbOpen;
    QSqlDatabase m_db;
public:
    DataBase();
    ~DataBase();

    bool createConnection();  //创建一个连接
    bool createTable();       //创建数据库表
    bool insertRecord(ClassModel classModel);      //新增数据
    bool queryAll();          //查询所有信息
    bool deleteAll();         //删除所有信息
};

#endif // DATABASE_H
