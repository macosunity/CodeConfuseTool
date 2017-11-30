#ifndef CPPPARSER_H
#define CPPPARSER_H

#include<string>
#include<fstream>
#include<vector>
#include<iterator>
#include<ctype.h>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "srcfilemodel.h"
#include "stringutil.h"
#include "classmodel.h"

using namespace std;

#define INCLUDE                 10
#define CLASS                   20
#define TEMPLATE_CLASS          30
#define VARIABLE                40
#define FUNCTION                50
#define UNDEINED                -1
#define NOTFOUND                0
#define HAVEFOUND               1

/**
 * 目前出现的问题:
 * 1. 不用制表符首行推进，而是用空格。函数没有识别到。
 */

class CppParser
{
public:
    string classname;//类的名字
    vector<string> extends;//类的extends
    vector<string> var;//类的变量
    vector<string> properties;//类的属性
    vector<string> function;//类的函数

    vector<string> include;//include集合
    vector<CppParser> _classes;//类的结合

    CppParser();

    int parseCppFile(SrcFileModel srcFile);

private:
    inline string& rtrim(string &str);
    inline string& ltrim(string &str);
    string& trim(string &s); //去除空格和换行
    int judge(string s);//判断字符串名字返回不同的值
    void D(string& str,char c);//在字符串str中循环删除字符c
    void D(string& str,string s);//删除所有指定的字符串
    void R(string& str);//以\r为判断删除注释
    vector<string> divideByTab(string &str);//以制表符为分隔符分解字符串成vector
    void ignorespacetab(const string& str,size_t& fI);//fI停在非空格和制表符处
    void ignorealnum(const string&str ,size_t& fI);//fI停在非数字和字母处
    void display(SrcFileModel fileModel);//用文件输出流输出
    int findSubStrAtPos(string& str,string s,size_t& pos);//在pos处，str找s
    string findClassName(const string& str,size_t &begin);//在一个字符串上找类名
    vector<string> findExtendsName(const string& str,size_t pos);//在一个字符串上找扩展名
    int findFunctionAndVarsOfClass(string& str,string s,size_t& pos,CppParser& theclass);

    int findGlobalClassDeclares(string& str);//寻找全局类声明，和友元类;

    int findGlobalVarsAndFunctions(string& str);//寻找全局变量和全局函数
    void actionscope_ignore(const string& str,size_t& fI);//忽略一个大的作用域中的所有作用域
    vector<size_t> actionscope(const string& str,size_t& fI);//获取最大的作用域的位置
    vector<string> split(std::string str,std::string pattern);

    bool is_str_contain_space(string str);//是否包含空格
    
    bool handleCppIdentify(ClassModel &classModel);
};


#endif // CPPPARSER_H
