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

void StringUtil::TrimLeft(string& str)
{
    str.erase(0,str.find_first_not_of(' '));
}
void StringUtil::TrimRight(string& str)
{
    str.erase(str.find_last_not_of(' ')+1);
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

    return str.compare(0,strStart.size(),strStart)==0?true:false;
}

bool StringUtil::EndWith(const string& str,const string& strEnd)
{
    if(str.empty() || strEnd.empty())
    {
        return false;
    }
    return str.compare(str.size()-strEnd.size(),strEnd.size(),strEnd)==0?true:false;
}
