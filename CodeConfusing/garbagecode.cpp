//
//  garbagecode.cpp
//  CodeConfusing
//
//  Created by Apple on 2017/12/4.
//
//

#include "garbagecode.h"
#include "stringutil.h"
#include <fstream>
#include <vector>
#include <QDebug>
#include <QFile>

using namespace std;

#warning 处理下 空函数 {}
void GarbageCodeTool::modifyContentInFile(const char *fileName, string content)
{
    StringUtil su;
    
    string line;
    string preline = "";
    
    ifstream fin(fileName);
    
    string tempStr = "";
    while(getline(fin,line,'\n'))
    {
        if(preline.length() > 0 &&
           line.length() > 0 &&
           su.EndWith(preline, ")") &&
           su.StartWith(line, "{"))
        {
            tempStr += content+"\n";
        }
        else
        {
            tempStr += line+"\n";
        }
        
        preline = line;
    }
    fin.close();
    
    ofstream fout(fileName, ofstream::out);
    fout<<tempStr;
    fout.close();
}

int GarbageCodeTool::insertGarbageCode(SrcFileModel srcFile, string garbageCode)
{
    modifyContentInFile(srcFile.filePath.c_str(), garbageCode);
    
    return 0;
}
