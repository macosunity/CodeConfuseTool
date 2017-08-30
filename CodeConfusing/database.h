#ifndef DATABASE_H
#define DATABASE_H

#include <QTextCodec>
#include <QTime>
#include <QtDebug>
#include <iterator>
#include <regex>
#include <string>
#include "classmodel.h"

using namespace std;

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
    vector<string> sqlcmds;
    vector<string> m_identifyVec;
    vector<ClassModel> m_modelVec;
public:
    DataBase();
    ~DataBase();
    
    void clearIdentifyVec();
    
    bool createConnection();  //创建一个连接
    bool createTable();       //创建数据库表
    bool insertRecord(ClassModel classModel);      //新增数据
    vector<string> queryAll();          //查询所有信息
    
    vector<ClassModel> queryAllModel();
    bool deleteAll();         //删除所有信息
};

#endif // DATABASE_H
