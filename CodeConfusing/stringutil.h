#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <regex>
using namespace std;

class StringUtil
{
public:
    StringUtil();
    
    bool is_allow_identify_name(string str);
    bool is_allow_identify_name_c_cpp(string str);
    bool is_var_or_function(string str);
    inline bool is_str_contain_chars(string str);
    inline bool is_str_contain_space(string str);
    
    void Toupper(string& str);                  //转大写
    void Tolower(string& str);                  //转小写
    
    inline string& rtrim(string &str);
    inline string& ltrim(string &str);
    string& trim(string &s);             //去除空格和换行
    void deleteSpecialChar(string& str);
    void DeleteChar(string& str,char ch);       //去掉字符串里某个字符，注意，是都去掉
    bool StartWith(const string& str,const string& strStart);       //字符串以某段开头
    bool EndWith(const string& str,const string& strEnd);           //字符串以某段结尾
};

#endif // STRINGUTIL_H
