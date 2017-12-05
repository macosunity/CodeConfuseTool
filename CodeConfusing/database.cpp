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

//向数据库中插入记录
bool DataBase::insertRecord(ClassModel classModel)
{
    StringUtil su;
    m_modelVec.push_back(classModel);
    m_identifyVec.push_back(classModel.identifyName);
    
    //类名
    if (classModel.isObjectiveC)
    {
        if (su.is_allow_identify_name(classModel.className))
        {
            m_identifyVec.push_back(su.trim(classModel.className));
        }
    }
    else
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
