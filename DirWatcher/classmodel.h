#ifndef CLASSMODEL_H
#define CLASSMODEL_H
#include <string>
using namespace std;

class ClassModel
{
public:
    string fileName; //不带后缀名的文件名
    string filePath; //文件路径
    string className;   //类名词
    string identifyName;

    ClassModel();
};

#endif // CLASSMODEL_H
