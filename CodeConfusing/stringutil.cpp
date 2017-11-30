#include "stringutil.h"

StringUtil::StringUtil()
{
}

void StringUtil::Toupper(string& str)
{
    transform(str.begin(),str.end(),str.begin(),::toupper);
}

void StringUtil::Tolower(string& str)
{
    transform(str.begin(),str.end(),str.begin(),::tolower);
}

inline string& StringUtil::ltrim(string &str)
{
    string::iterator p = find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(isspace)));
    str.erase(str.begin(), p);
    return str;
}

inline string& StringUtil::rtrim(string &str)
{
    string::reverse_iterator p = find_if(str.rbegin(), str.rend(), not1(ptr_fun<int , int>(isspace)));
    str.erase(p.base(), str.end());
    return str;
}

string& StringUtil::trim(string &str)
{
    ltrim(rtrim(str));
    return str;
}

void StringUtil::DeleteChar(string& str,char ch)
{
    str.erase(remove_if(str.begin(),str.end(),bind2nd(equal_to<char>(), ch)),str.end());
}

bool StringUtil::StartWith(const string& str,const string& strStart)
{
    if(str.empty() || strStart.empty())
    {
        return false;
    }
    if (strStart.size() > str.size())
    {
        return false;
    }

    return str.compare(0,strStart.size(),strStart)==0?true:false;
}

bool StringUtil::EndWith(const string& str,const string& strEnd)
{
    if(str.empty() || strEnd.empty())
    {
        return false;
    }
    
    if (strEnd.size() > str.size())
    {
        return false;
    }
    return str.compare(str.size()-strEnd.size(),strEnd.size(),strEnd)==0?true:false;
}

void StringUtil::deleteSpecialChar(string& str)
{
    str = trim(str);
    
    if(str.find('{') != string::npos)
    {
        size_t index_s = 0;
        while(index_s<str.length())
        {
            index_s = str.find('{',index_s);//找 '{' 的位置
            if(index_s != string::npos)
            {
                str.replace(index_s, 1, "");
            }
        }
    }
    
    if(str.find('*') != string::npos)
    {
        size_t index_s = 0;
        while(index_s<str.length())
        {
            index_s = str.find('*',index_s);//找 '*' 的位置
            if(index_s != string::npos)
            {
                str.replace(index_s, 1, "");
            }
        }
    }
    
    if(str.find('~') != string::npos)
    {
        size_t index_s = 0;
        while(index_s<str.length())
        {
            index_s = str.find('~',index_s);//找 '~' 的位置
            if(index_s != string::npos)
            {
                str.replace(index_s, 1, "");
            }
        }
    }
    
    if(str.find('&') != string::npos)
    {
        size_t index_s = 0;
        while(index_s<str.length())
        {
            index_s = str.find('&',index_s);//找 '&' 的位置
            if(index_s != string::npos)
            {
                str.replace(index_s, 1, "");
            }
        }
    }
    
    size_t bracket_left_index = str.find('[');
    if (bracket_left_index != string::npos)
    {
        str = str.substr(0, bracket_left_index);
    }
}

inline bool StringUtil::is_str_contain_space(string str)
{
    return str.find(' ') != string::npos;
}

inline bool StringUtil::is_str_contain_chars(string str)
{
    bool is_contain = false;
    
    vector<string> chars_vec;
    chars_vec.push_back("public:");
    chars_vec.push_back("private:");
    chars_vec.push_back("protected:");
    chars_vec.push_back("#");
    chars_vec.push_back("=");
    chars_vec.push_back(":");
    chars_vec.push_back("if ");
    chars_vec.push_back("if(");
    chars_vec.push_back("return ");
    chars_vec.push_back("cout<<");
    chars_vec.push_back("cout ");
    chars_vec.push_back(".");
    chars_vec.push_back("this");
    chars_vec.push_back("->");
    chars_vec.push_back("\\");
    
    for(vector<string>::iterator iter = chars_vec.begin(); iter!=chars_vec.end();++iter)
    {
        if (str.find(*iter) != string::npos)
        {
            is_contain = true;
            break;
        }
    }
    
    return is_contain;
}

bool StringUtil::is_var_or_function(string str)
{
    return (str.find(';') == string::npos && is_str_contain_space(str) && !is_str_contain_chars(str));
}

bool StringUtil::is_allow_identify_name(string str)
{
    if (str.length() == 1)
    {
        return false;
    }
    
    StringUtil stringUtil;
    regex reg("[_[:alpha:]][_[:alnum:]]*");
    
    regex upper_underline_reg("[_[:digit:][:upper:]]*");
    
    string judge_str = stringUtil.trim(str);
    if (regex_match(str, reg) && !regex_match(str, upper_underline_reg) && !stringUtil.StartWith(str, "_") && !stringUtil.StartWith(str, "init") && !stringUtil.StartWith(str, "dispatch_") && !stringUtil.StartWith(str, "gl") && !stringUtil.StartWith(str, "const_") && !stringUtil.StartWith(str, "objc_") && !stringUtil.StartWith(str, "CC_") && !stringUtil.StartWith(str, "CG") && !stringUtil.StartWith(str, "CM") && !stringUtil.StartWith(str, "CT") && !stringUtil.StartWith(str, "CF") && !stringUtil.StartWith(str, "NS") && !stringUtil.StartWith(str, "sqlite3_") && !stringUtil.StartWith(str, "set") && !stringUtil.StartWith(str, "is") && !stringUtil.StartWith(str, "NS") && !stringUtil.StartWith(str, "kCG") && !stringUtil.StartWith(str, "AV") && !stringUtil.StartWith(str, "kCF") && !stringUtil.StartWith(str, "kCT") && !stringUtil.StartWith(str, "isEqual") && !stringUtil.StartWith(str, "UI") && !stringUtil.StartWith(str, "Sec") && !stringUtil.StartWith(str, "error") && !stringUtil.EndWith(str, "error") && !stringUtil.StartWith(str, "unsigned"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool StringUtil::is_allow_identify_name_c_cpp(string str)
{
    if (str.length() == 1)
    {
        return false;
    }
    
    StringUtil stringUtil;
    regex reg("[_[:alpha:]][_[:alnum:]]*");
    
    regex upper_underline_reg("[_[:digit:][:upper:]]*");
    
    string judge_str = stringUtil.trim(str);
    if (regex_match(str, reg) && !regex_match(str, upper_underline_reg) && !stringUtil.StartWith(str, "_") && !stringUtil.StartWith(str, "gl") && !stringUtil.StartWith(str, "const_") && !stringUtil.StartWith(str, "objc_") )
    {
        return true;
    }
    else
    {
        return false;
    }
}
