#ifndef SRCFILEMODEL_H
#define SRCFILEMODEL_H

#include <string>

using namespace std;

class SrcFileModel
{
public:
    string fileName;
    string filePath;
    string headerFileName;
    string headerFilePath;
    string cFileName;
    string cFilePath;
    string cppFileName;
    string cppFilePath;
    string mFileName;
    string mFilePath;
    string mmFileName;
    string mmFilePath;

    bool isParsed;  //是否已进行过语法分析 true-是 false-否

    SrcFileModel();
};

#endif // SRCFILEMODEL_H
