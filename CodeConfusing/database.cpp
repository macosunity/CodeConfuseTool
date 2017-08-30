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

//向数据库中插入记录
bool DataBase::insertRecord(ClassModel classModel)
{
    QUuid id = QUuid::createUuid();
    QString strId = id.toString();
    
    StringUtil su;
    if (classModel.isObjectiveC)
    {
        m_modelVec.push_back(classModel);
        m_identifyVec.push_back(classModel.identifyName);
    }
    else
    {
        if (su.is_var_or_function(classModel.identifyName))
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
            
            su.deleteSpecialChar(identify_str);
            if (su.is_allow_identify_name(identify_str))
            {
                m_modelVec.push_back(classModel);
                m_identifyVec.push_back(su.trim(identify_str));
            }
        }
    }
    
    //类名
    if (su.is_allow_identify_name(classModel.className))
    {
        m_identifyVec.push_back(su.trim(classModel.className));
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
