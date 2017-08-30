#include "classmodel.h"

ClassModel::ClassModel()
{
    fileName = ""; //不带后缀名的文件名
    filePath = ""; //文件路径
    className = "";   //类名词
    identifyName = "";
    identifyOriginName = "";
    isObjectiveC = false;
    isPropertyName = false;
    isMethodName = false;
}
