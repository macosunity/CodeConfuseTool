#include "database.h"
#include <QDebug>
#include <QUuid>
#include <QDesktopServices>
#include <QDir>
#include "stringutil.h"

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

//找到属性和方法名字相同，则全部删除
size_t is_property_method_same(string identify_str, bool isProperty)
{
    DataBase *database = DataBase::Instance();
    
    vector<ClassModel> modelVec = database->queryAllModel();
    
    StringUtil stringUtil;
    
    if (isProperty)
    {
        for (size_t i=0; i<modelVec.size(); i++)
        {
            ClassModel model = modelVec[i];
            if (stringUtil.StartWith(model.identifyName, identify_str) && model.identifyName.length() == identify_str.length() && model.isObjectiveC && model.isMethodName)
            {
                return i;
            }
        }
    }
    else
    {
        for (size_t i=0; i<modelVec.size(); i++)
        {
            ClassModel model = modelVec[i];
            if (stringUtil.StartWith(model.identifyName, identify_str) && model.identifyName.length() == identify_str.length() && model.isObjectiveC && model.isPropertyName)
            {
                return i;
            }
        }
    }
    
    return -1;
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
    if (str.length() == 1)
    {
        return false;
    }
    
    StringUtil stringUtil;
    regex reg("[_[:alpha:]][_[:alnum:]]*");
    
    regex upper_underline_reg("[_[:upper:]]*");
    
    string judge_str = trim(str);
    if (regex_match(str, reg) && !regex_match(str, upper_underline_reg) && !stringUtil.StartWith(str, "_") && !stringUtil.StartWith(str, "initWith") && !stringUtil.StartWith(str, "dispatch_") && !stringUtil.StartWith(str, "gl") && !stringUtil.StartWith(str, "const_") && !stringUtil.StartWith(str, "objc_") && !stringUtil.StartWith(str, "CC_") && !stringUtil.StartWith(str, "CG") && !stringUtil.StartWith(str, "CM") && !stringUtil.StartWith(str, "CT") && !stringUtil.StartWith(str, "CF") && !stringUtil.StartWith(str, "NS") && !stringUtil.StartWith(str, "sqlite3_") && !stringUtil.StartWith(str, "set") && !stringUtil.StartWith(str, "NS") && !stringUtil.StartWith(str, "kCG") && !stringUtil.StartWith(str, "AV") && !stringUtil.StartWith(str, "kCF") && !stringUtil.StartWith(str, "kCT") && !stringUtil.StartWith(str, "isEqual") && !stringUtil.StartWith(str, "UI") && !stringUtil.StartWith(str, "Sec") && !stringUtil.StartWith(str, "error") && !stringUtil.EndWith(str, "error") && !stringUtil.StartWith(str, "unsigned"))
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

bool DataBase::handleObjectiveCIdentify(ClassModel classModel)
{
    StringUtil stringUtil;
    
    string identify_str = classModel.identifyName;
    
    size_t UI1 = identify_str.find("UI_APPEARANCE_SELECTOR");
    if (UI1 != string::npos)
    {
        identify_str = identify_str.substr(0, UI1);
    }
    
    size_t operator_index = identify_str.find(" operator");
    size_t operator_index2 = identify_str.find("::operator");
    size_t method_index = identify_str.find('+');
    size_t method_index2 = identify_str.find('-');
    
    size_t property_index = identify_str.find("@property");
    if ( (method_index != string::npos || method_index2 != string::npos) &&
        (operator_index==string::npos && operator_index2==string::npos) )//Objective C Method
    {
        if (stringUtil.StartWith(identify_str, "set"))
        {
            return false;
        }
        
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
            string method_str = trim(identify_str);
            int index = is_property_method_same(method_str, false);
            if (index == -1)
            {
                if (is_allow_identify_name(classModel.className))
                {
                    //qDebug() << "handle 发现方法:" << classModel.identifyName.c_str() << identify_str.c_str();
                    m_identifyVec.push_back(method_str);
                    
                    classModel.identifyName = method_str;
                    classModel.isMethodName = true;
                    m_modelVec.push_back(classModel);
                }
            }
            else
            {
                vector<string>::iterator it = m_identifyVec.begin()+index;
                m_identifyVec.erase(it);
                
                vector<ClassModel>::iterator modelIt = m_modelVec.begin()+index;
                m_modelVec.erase(modelIt);
                
                for(vector<string>::iterator id_it=m_identifyVec.begin();id_it!=m_identifyVec.end();)
                {
                    string iden = *id_it;
                    if(stringUtil.StartWith(iden, method_str) && iden.length() == method_str.length())
                    {
                        id_it=m_identifyVec.erase(id_it);
                    }
                    else
                    {
                        ++id_it;
                    }
                }
                
                for(vector<ClassModel>::iterator model_it=m_modelVec.begin();model_it!=m_modelVec.end();)
                {
                    ClassModel model = *model_it;
                    if(stringUtil.StartWith(model.identifyName, method_str) && model.identifyName.length() == method_str.length())
                    {
                        model_it=m_modelVec.erase(model_it);
                    }
                    else
                    {
                        ++model_it;
                    }
                }
            }
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
            return false;
        }
        
        size_t last_space_index = identify_str.find_last_of(' ');
        if (last_space_index != string::npos)
        {
            identify_str = identify_str.substr(last_space_index, identify_str.length()-last_space_index);
        }
        
        deleteSpecialChar(identify_str);
        if (is_allow_identify_name(identify_str))
        {
            string property_str = trim(identify_str);
            
            int index = is_property_method_same(property_str, true);
            if (index == -1)
            {
                if (is_allow_identify_name(classModel.className))
                {
                    //qDebug() << "handle 发现属性:" << classModel.identifyName.c_str()  << property_str.c_str() << endl;
                    m_identifyVec.push_back(property_str);
                    
                    classModel.identifyName = property_str;
                    classModel.isPropertyName = true;
                    m_modelVec.push_back(classModel);
                }
            }
            else
            {
                vector<string>::iterator it = m_identifyVec.begin()+index;
                m_identifyVec.erase(it);
                
                vector<ClassModel>::iterator modelIt = m_modelVec.begin()+index;
                m_modelVec.erase(modelIt);
                
                for(vector<string>::iterator id_it=m_identifyVec.begin();id_it!=m_identifyVec.end();)
                {
                    string iden = *id_it;
                    if(stringUtil.StartWith(iden, property_str) && iden.length() == property_str.length())
                    {
                        id_it=m_identifyVec.erase(id_it);
                    }
                    else
                    {
                        ++id_it;
                    }
                }
                
                for(vector<ClassModel>::iterator model_it=m_modelVec.begin();model_it!=m_modelVec.end();)
                {
                    ClassModel model = *model_it;
                    if(stringUtil.StartWith(model.identifyName, property_str) && model.identifyName.length() == property_str.length())
                    {
                        model_it=m_modelVec.erase(model_it);
                    }
                    else
                    {
                        ++model_it;
                    }
                }
            }
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
//            qDebug() << "发现其他1:" << classModel.identifyName.c_str() << identify_str.c_str();
        }
    }
    
    return true;
}

//向数据库中插入记录
bool DataBase::insertRecord(ClassModel classModel)
{
    QUuid id = QUuid::createUuid();
    QString strId = id.toString();
    
    StringUtil su;
    if (is_var_or_function(classModel.identifyName))
    {
        if (classModel.isObjectiveC)
        {
            handleObjectiveCIdentify(classModel);
        }
        else
        {
            string identify_str = classModel.identifyName;

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
                //qDebug() << "发现其他2:" << classModel.identifyName.c_str() << identify_str.c_str();
                m_modelVec.push_back(classModel);
                m_identifyVec.push_back(trim(identify_str));
            }
        }
    }
    else
    {
        handleObjectiveCIdentify(classModel);
    }
    
    //类名
    if (is_allow_identify_name(classModel.className))
    {
        string identify_str = trim(classModel.className);
        m_identifyVec.push_back(identify_str);
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

vector<ClassModel> DataBase::queryAllModel()
{
    return m_modelVec;
}

//删除所有记录
bool DataBase::deleteAll()
{
    return true;
}
