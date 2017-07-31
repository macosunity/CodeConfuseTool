#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <algorithm>
#include <functional>
#include <string>
using namespace std;

class StringUtil
{
public:
    StringUtil();

    void Toupper(string& str);                  //转大写
    void Tolower(string& str);                  //转小写
    void TrimRight(string& str);                //去除后面的空格
    void TrimLeft(string& str);                 //去除前面的空格
    void DeleteChar(string& str,char ch);       //去掉字符串里某个字符，注意，是都去掉
    bool StartWith(const string& str,const string& strStart);       //字符串以某段开头
    bool EndWith(const string& str,const string& strEnd);           //字符串以某段结尾
};

#endif // STRINGUTIL_H
