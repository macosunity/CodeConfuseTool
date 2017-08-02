#ifndef OCPARSER_H
#define OCPARSER_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <map>
#include <ctype.h>
#include <stdio.h>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include "srcfilemodel.h"
using namespace std;

#define OC_INCLUDE     8
#define OC_IMPORT      7
#define OC_CLASS       3
#define OC_IMPLEMENTS  2
#define OC_VARIABLE    4
#define OC_FUNCTION    5
#define OC_UNDEINED    -1
#define OC_NOTFOUND    0
#define OC_HAVEFOUND   1

typedef map<string, vector<string > > STRING2VECTOR;

class OCParser
{
public:
    string classname;//类的名字
    string extends;//类的extends
    vector<string> delegates;//类的delegates
    vector<string> var;//类的变量
    vector<string> properties;//类的属性
    vector<string> function;//类的函数

    vector<string> oc_include;//include集合
    vector<string> oc_import;//import集合
    vector<OCParser> _oc;//类的结合

    OCParser();
    OCParser(string extends);

    int parseOCFile(SrcFileModel srcFile);

private:

    inline string& rtrim(string &str);
    inline string& ltrim(string &str);
    string& trim(string &s); //去除空格和换行
    int judge(string s);//判断字符串名字返回不同的值
    void D(string& str,char c);//在字符串str中循环删除字符c
    void R(string& str);//以\r为判断删除注释
    vector<string> divideByTab(string &str);//以制表符为分隔符分解字符串成vector
    void ignorespacetab(const string& str,size_t& fI);//fI停在非空格和制表符处
    void ignorealnum(const string&str ,size_t& fI);//fI停在非数字和字母处
    void display(SrcFileModel fileModel);//用文件输出流输出
    int find(string& str,string s,int& pos);//在pos处，str找s
    string findClassName(const string& str,size_t &begin);//在一个字符串上找类名
    string findExtendsName(const string& str,int pos);//在一个字符串上找扩展名
    const vector<string> findDelegatesName(const string& str,int pos);//代理
    const map<string,vector<string>> findPropertiesAndFunctionDeclaresName(const string& str,int pos);//属性和方法
    void actionscope_ignore(const string& str,size_t& fI);//忽略一个大的作用域中的所有作用域
    vector<int> actionscope(const string& str,size_t& fI);//获取最大的作用域的位置
};

#endif // OCPARSER_H
