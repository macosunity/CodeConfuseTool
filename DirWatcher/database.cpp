#include "database.h"
#include <QDebug>
#include <QUuid>
#include <QDesktopServices>
#include <QDir>

DataBase::DataBase()
{
    m_isDbOpen = createConnection();
}

DataBase::~DataBase()
{
    if(m_isDbOpen)
    {
        m_db.close();
    }
}
//建立一个数据库连接
bool DataBase::createConnection()
{
    //以后就可以用"classes_db"与数据库进行连接了

    //与数据库建立连接
    if (QSqlDatabase::contains("classes_db"))
    {
        m_db = QSqlDatabase::database("classes_db");
    }
    else
    {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "classes_db");
    }
    //设置数据库名

    QString dbFilePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

    QDir *dbDir = new QDir;
    bool isDbDirExist = dbDir->exists(dbFilePath);
    if(isDbDirExist)
    {
        qDebug() << "文件夹已经存在！" << endl;
    }
    else
    {
        bool ok = dbDir->mkdir(dbFilePath);
        if( ok )
        {
            qDebug() << "文件夹创建成功！" << endl;
        }
    }
    dbFilePath = dbFilePath.append("/classes_info.db");
    qDebug() << "数据库地址：" << dbFilePath << endl;
    m_db.setDatabaseName(dbFilePath);
    if( !m_db.open())
    {
        qDebug() << "无法建立数据库连接";
        return false;
    }
    qDebug() << "建立数据库连接成功！";
    return true;
}

//创建数据库表
bool DataBase::createTable()
{
    if(!m_db.isOpen())
    {
        createConnection();
    }

    QSqlQuery query(m_db);
    bool success = query.exec("create table if not exists classes_table("
                              "[id] varchar PRIMARY KEY,"
                              "[file_path] varchar,[file_name] varchar,"
                              "[class_name] varchar,[identify_name] varchar)");
    if(success)
    {
        qDebug() << QObject::tr("数据库表创建成功！\n");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("数据库表创建失败！\n");
        return false;
    }
}

inline bool DataBase::is_str_contain_space(string str)
{
    return str.find(' ') != string::npos;
}

inline bool DataBase::is_str_contain_chars(string str)
{
    bool is_contain = false;
    
    vector<string> chars_vec;
    chars_vec.push_back("public:");
    chars_vec.push_back("private:");
    chars_vec.push_back("protected:");
    chars_vec.push_back("#");
    chars_vec.push_back("=");
    chars_vec.push_back(":");
    chars_vec.push_back("if ");
    chars_vec.push_back("if(");
    chars_vec.push_back("return ");
    chars_vec.push_back("cout<<");
    chars_vec.push_back("cout ");
    chars_vec.push_back(".");
    chars_vec.push_back("this");
    chars_vec.push_back("->");
    chars_vec.push_back("\\");
    
    for(vector<string>::iterator iter = chars_vec.begin(); iter!=chars_vec.end();++iter)
    {
        if (str.find(*iter) != string::npos)
        {
            is_contain = true;
            break;
        }
    }
    
    return is_contain;
}

inline bool DataBase::is_var_or_function(string str)
{
    return (str.find(';') == string::npos && is_str_contain_space(str) && !is_str_contain_chars(str));
}

inline bool DataBase::is_allow_identify_name(string str)
{
    return (str.find('(') == string::npos && str.find(')') == string::npos);
}

inline string& DataBase::ltrim(string &str)
{
    string::iterator p = find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(isspace)));
    str.erase(str.begin(), p);
    return str;
}

inline string& DataBase::rtrim(string &str)
{
    string::reverse_iterator p = find_if(str.rbegin(), str.rend(), not1(ptr_fun<int , int>(isspace)));
    str.erase(p.base(), str.end());
    return str;
}

inline string& DataBase::trim(string &str)
{
    ltrim(rtrim(str));
    return str;
}

inline void DataBase::deleteChar(string& str,char c)
{
    if(c == '*')
    {
        size_t index_s = 0;
        while(index_s<str.length())
        {
            index_s = str.find(c,index_s);//找 '#' 的位置
            if(index_s != string::npos)
            {
                str.erase(index_s);
            }
        }
        return;
    }
}

//向数据库中插入记录
bool DataBase::insertRecord(ClassModel classModel)
{
    return true;
    QUuid id = QUuid::createUuid();
    QString strId = id.toString();

//    string sql = "insert into classes_table values('" + strId.toStdString() + "','" + classModel.filePath + "','" + classModel.fileName+ "','"  + classModel.className+ "','" + classModel.identifyName+"')";
//    qDebug() << sql.c_str() << endl;
    
    
    if (is_var_or_function(classModel.identifyName))
    {
        string identify_str = classModel.identifyName;
        
        if (classModel.isObjectiveC)
        {
            size_t operator_index = identify_str.find(" operator");
            size_t operator_index2 = identify_str.find("::operator");
            size_t method_index = identify_str.find('+');
            size_t method_index2 = identify_str.find('-');
            
            size_t property_index = identify_str.find("@property");
            if ( (method_index != string::npos || method_index2 != string::npos) &&
                 (operator_index==string::npos && operator_index2==string::npos) )//Objective C Method
            {
                size_t first_colon_index = identify_str.find_first_of(':');
                if (first_colon_index != string::npos)
                {
                    identify_str = identify_str.substr(0, first_colon_index);
                }
                
                size_t first_brackets_index = identify_str.find_last_of(')');
                if (first_brackets_index != string::npos)
                {
                    identify_str = identify_str.substr(first_brackets_index+1, identify_str.length()-first_brackets_index);
                }
                
                if (is_allow_identify_name(identify_str))
                {
                    deleteChar(identify_str, '*');
                    m_identifyVec.push_back(trim(identify_str));
                }
            }
            else if(property_index != string::npos)//Objective C Property
            {
                size_t block_index = identify_str.find_first_of('^');
                if (block_index != string::npos)
                {
                    identify_str = identify_str.substr(block_index+1, identify_str.length()-block_index);
                    size_t first_brackets_index = identify_str.find_first_of(')');
                    if (first_brackets_index != string::npos)
                    {
                        identify_str = identify_str.substr(0, first_brackets_index);
                    }
                }
                
                size_t last_space_index = identify_str.find_last_of(' ');
                if (last_space_index != string::npos)
                {
                    identify_str = identify_str.substr(last_space_index, identify_str.length()-last_space_index);
                }
                
                if (is_allow_identify_name(identify_str))
                {
                    deleteChar(identify_str, '*');
                    m_identifyVec.push_back(trim(identify_str));
                }
            }
            else
            {
                size_t last_brackets_index = identify_str.find_first_of('(');
                if (last_brackets_index != string::npos)
                {
                    identify_str = identify_str.substr(0, last_brackets_index);
                }
                
                size_t last_space_index = identify_str.find_last_of(' ');
                if (last_space_index != string::npos)
                {
                    identify_str = identify_str.substr(last_space_index, identify_str.length()-last_space_index);
                }
                
                if (is_allow_identify_name(identify_str))
                {
                    deleteChar(identify_str, '*');
                    m_identifyVec.push_back(trim(identify_str));
                }
            }
        }
        else
        {
            size_t last_brackets_index = identify_str.find_last_of('(');
            if (last_brackets_index != string::npos)
            {
                identify_str = identify_str.substr(0, last_brackets_index);
            }
            
            size_t last_space_index = identify_str.find_last_of(' ');
            if (last_space_index != string::npos)
            {
                identify_str = identify_str.substr(last_space_index, identify_str.length()-last_space_index);
            }
            
            if (is_allow_identify_name(identify_str))
            {
                deleteChar(identify_str, '*');
                m_identifyVec.push_back(trim(identify_str));
            }
        }
    }
//    if(!m_db.isOpen())
//    {
//        createConnection();
//    }

//    QSqlQuery query(m_db);
//    query.prepare("insert into classes_table values(:id,:file_path,:file_name,:class_name,:identify_name,:file_type,:identify_type)");
//
//    query.bindValue(":id", strId);
//    query.bindValue(":file_path", classModel.filePath.c_str());
//    query.bindValue(":file_name", classModel.fileName.c_str());
//    query.bindValue(":class_name", classModel.className.c_str());
//    query.bindValue(":identify_name", classModel.identifyName.c_str());
//    query.bindValue(":file_type", classModel.fileType);
//    query.bindValue(":identify_type", classModel.identifyType);
//
//    bool success=query.exec();
//    if(!success)
//    {
//        QSqlError lastError = query.lastError();
//        qDebug() << lastError.driverText() << QString(QObject::tr("插入失败"));
//        return false;
//    }
    return true;
}

//查询所有信息
vector<string> DataBase::queryAll()
{
    return m_identifyVec;
//    if(!m_db.isOpen())
//    {
//        createConnection();
//    }
//
//    QSqlQuery query(m_db);
//    query.prepare("select distinct(identify_name) from classes_table");
//    if(!query.exec())
//    {
//        qDebug()<<query.lastError();
//    }
//    else
//    {
//        while(query.next())
//        {
//            qDebug() << query.value(0).toString() << " ";
//            qDebug() << "\n";
//        }
//    }
    
//    return true;
}

//删除所有记录
bool DataBase::deleteAll()
{
    if(!m_db.isOpen())
    {
        createConnection();
    }

    QSqlQuery query(m_db);
    query.prepare(QString("delete from classes_table"));
    if(!query.exec())
    {
        qDebug() << "删除记录失败！";
        return false;
    }
    return true;
}
