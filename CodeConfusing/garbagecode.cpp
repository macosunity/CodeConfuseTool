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

void GarbageCodeTool::modifyContentInFile(const char *fileName, string content)
{
    StringUtil su;
    
    string line;
    string preline = "";
    
    ifstream fin(fileName);
    
    string tempStr = "";
    while(getline(fin,line,'\n'))
    {
        preline = su.trim(preline);
        if(preline.length() > 0 &&
           line.length() > 0 &&
           su.EndWith(preline, ")") &&
           su.StartWith(line, "{"))
        {
            string subline = line.substr(1,line.length()-1);
            tempStr += + "{\n\t" + content+"\n" + subline + "\n";
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

void GarbageCodeTool::insertGarbageCode(SrcFileModel srcFile, string garbageCode)
{
    modifyContentInFile(srcFile.filePath.c_str(), garbageCode);
}

void GarbageCodeTool::insertIncludeCode(SrcFileModel srcFile, string includeCode)
{
    StringUtil su;

    string line;
    string preline = "";

    const char *fileName = srcFile.filePath.c_str();
    ifstream fin(fileName);

    bool isHeadInjected = false;
    string tempStr = "";
    while(getline(fin,line,'\n'))
    {
        if(line.length() > 0 &&
           su.StartWith(line, "#include") &&
           isHeadInjected == false)
        {
            string subline = line.substr(0,line.length());
            qDebug() << "subline:" << subline.c_str()<< endl;
            tempStr += + "\n" + includeCode+"\n"+subline+"\n";

            isHeadInjected = true;
        }
        else
        {
            tempStr += line+"\n";
        }
    }
    fin.close();

    ofstream fout(fileName, ofstream::out);
    fout << tempStr;
    fout.close();
}
