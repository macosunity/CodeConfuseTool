#include "database.h"
#include <QDebug>
#include <QUuid>
#include <QDesktopServices>
#include <QDir>

DataBase::DataBase()
{
}

DataBase::~DataBase()
{
}

void DataBase::clearIdentifyVec()
{
    vector<string>().swap(m_identifyVec);
}

//建立一个数据库连接
bool DataBase::createConnection()
{
    return true;
}

//创建数据库表
bool DataBase::createTable()
{
    return true;
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
    regex reg("[_[:alpha:]][_[:alnum:]]*");
    
    string judge_str = trim(str);
    if (regex_match(str, reg))
    {
        return true;
    }
    else
    {
        return false;
    }
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

inline void DataBase::deleteSpecialChar(string& str)
{
    str = trim(str);
    
    if(str.find('{') != string::npos)
    {
        size_t index_s = 0;
        while(index_s<str.length())
        {
            index_s = str.find('{',index_s);//找 '{' 的位置
            if(index_s != string::npos)
            {
                str.erase(index_s);
            }
        }
    }
    
    if(str.find('*') != string::npos)
    {
        size_t index_s = 0;
        while(index_s<str.length())
        {
            index_s = str.find('*',index_s);//找 '*' 的位置
            if(index_s != string::npos)
            {
                str.erase(index_s);
            }
        }
    }
    
    if(str.find('~') != string::npos)
    {
        size_t index_s = 0;
        while(index_s<str.length())
        {
            index_s = str.find('~',index_s);//找 '~' 的位置
            if(index_s != string::npos)
            {
                str.erase(index_s);
            }
        }
    }
    
    if(str.find('&') != string::npos)
    {
        size_t index_s = 0;
        while(index_s<str.length())
        {
            index_s = str.find('&',index_s);//找 '&' 的位置
            if(index_s != string::npos)
            {
                str.erase(index_s);
            }
        }
    }
    
    size_t bracket_left_index = str.find('[');
    if (bracket_left_index != string::npos)
    {
        str = str.substr(0, bracket_left_index);
    }
}

//向数据库中插入记录
bool DataBase::insertRecord(ClassModel classModel)
{
    QUuid id = QUuid::createUuid();
    QString strId = id.toString();
    
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
                
                deleteSpecialChar(identify_str);
                if (is_allow_identify_name(identify_str))
                {
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
                
                deleteSpecialChar(identify_str);
                if (is_allow_identify_name(identify_str))
                {
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
                
                deleteSpecialChar(identify_str);
                if (is_allow_identify_name(identify_str))
                {
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
            
            deleteSpecialChar(identify_str);
            if (is_allow_identify_name(identify_str))
            {
                m_identifyVec.push_back(trim(identify_str));
            }
        }
    }
    return true;
}

//查询所有信息
vector<string> DataBase::queryAll()
{
    sort(m_identifyVec.begin(),m_identifyVec.end());
    m_identifyVec.erase(unique(m_identifyVec.begin(), m_identifyVec.end()), m_identifyVec.end());
    return m_identifyVec;
}

//删除所有记录
bool DataBase::deleteAll()
{
    return true;
}