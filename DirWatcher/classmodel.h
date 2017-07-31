#ifndef CLASSMODEL_H
#define CLASSMODEL_H
#include <string>
using namespace std;

typedef enum
{
    C,
    Cpp,
    Header,
    ObjectiveC,
    ObjectiveCpp    //Objective C++
}FileType;

typedef enum
{
    Var,
    Property,
    Function,
    ClassName
}IdentifyType;

class ClassModel
{
public:
    string fileName; //不带后缀名的名词
    string filePath; //文件路径
    string className;   //类名词
    FileType fileType;    //文件类型 "c" "c++" "header" "oc" "oc++"
    string identifyName;
    IdentifyType identifyType;

    ClassModel();
};

#endif // CLASSMODEL_H
