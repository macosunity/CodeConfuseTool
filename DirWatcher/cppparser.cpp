#include "cppparser.h"
#include "database.h"
#include <QDebug>

CppParser::CppParser()
{
    classname = "";
}

int CppParser::parseCppFile(SrcFileModel srcFile)
{
    QFile file(srcFile.filePath.c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    StringUtil stringUtil;
    if(stringUtil.EndWith(srcFile.fileName, ".h") && srcFile.mFileName.length() == 0)
    {
        ifstream fin(srcFile.filePath.c_str());//文件输入流，p是代码路径
        string str;
        string temp;
        size_t pos = 0;
        while(getline(fin,temp,'\n'))
        {
            str.append(temp+"\r\n\t");
        }
        R(str);//删除全部注释，跟D(temp)不一样的是 D(temp)以\t判断，这个以\r判断
        //注释里面出现include的话会出事囧
        while(findSubStrAtPos(str,"#include",pos)){}//连续读取代码中的include名
        while(findSubStrAtPos(str,"class ",pos)){}//连续读取代码中的类
        while(findSubStrAtPos(str,"template",pos)){}//连续读取代码中的类

        QFile cppFile(srcFile.cppFilePath.c_str());
        if(cppFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            fin.close();

            srcFile.fileName = srcFile.cppFileName;
            srcFile.filePath = srcFile.cppFilePath;
            ifstream fin_cpp(srcFile.cppFilePath.c_str());//文件输入流，p是代码路径
            string str_cpp;
            string temp_cpp;
            pos = 0;
            while(getline(fin_cpp,temp_cpp,'\n'))
            {
                str_cpp.append(temp_cpp+"\r\n\t");
            }
            while(findSubStrAtPos(str_cpp,"#include",pos)){}//连续读取代码中的include名

            R(str_cpp);//删除全部注释，跟D(temp)不一样的是 D(temp)以\t判断，这个以\r判断
            for(vector<CppParser>::iterator b = _classes.begin(); b!=_classes.end(); ++b)
            {
                while(findFunctionAndVarsOfClass(str_cpp, b->classname,pos, *b)){}//连续读取代码中的类
            }
            display(srcFile);
            fin_cpp.close();
        }
        else
        {
            display(srcFile);
            fin.close();
        }
    }
    else if(stringUtil.EndWith(srcFile.fileName, ".cpp") || stringUtil.EndWith(srcFile.fileName, ".cxx") || stringUtil.EndWith(srcFile.fileName, ".cc") || stringUtil.EndWith(srcFile.fileName, ".mm"))
    {
        QFile headFile(srcFile.headerFilePath.c_str());
        if(headFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ifstream fHeaderIn(srcFile.headerFilePath.c_str());//文件输入流，p是代码路径
            string str;
            string temp;
            size_t pos = 0;
            while(getline(fHeaderIn,temp,'\n'))
            {
                str.append(temp+"\r\n\t");
            }
            R(str);//删除全部注释，跟D(temp)不一样的是 D(temp)以\t判断，这个以\r判断
            //注释里面出现include的话会出事囧
            while(findSubStrAtPos(str,"#include",pos)){}//连续读取代码中的include名
            while(findSubStrAtPos(str,"class ",pos)){}//连续读取代码中的类
            while(findSubStrAtPos(str,"template",pos)){}//连续读取代码中的类
            fHeaderIn.close();

            ifstream fin_cpp(srcFile.filePath.c_str());//文件输入流，p是代码路径
            string str_cpp;
            string temp_cpp;
            pos = 0;
            while(getline(fin_cpp,temp_cpp,'\n'))
            {
                str_cpp.append(temp_cpp+"\r\n\t");
            }
            while(findSubStrAtPos(str_cpp,"#include",pos)){}//连续读取代码中的include名

            R(str_cpp);//删除全部注释，跟D(temp)不一样的是 D(temp)以\t判断，这个以\r判断
            for(vector<CppParser>::iterator b = _classes.begin(); b!=_classes.end(); ++b)
            {
                while(findFunctionAndVarsOfClass(str_cpp, b->classname,pos, *b)){}//连续读取代码中的类
            }
            display(srcFile);
            fin_cpp.close();
        }
        else
        {
            ifstream fin(srcFile.filePath.c_str());//文件输入流，p是代码路径
            string str;
            string temp;
            size_t pos = 0;
            while(getline(fin,temp,'\n'))
            {
                str.append(temp+"\r\n\t");
            }
            R(str);//删除全部注释，跟D(temp)不一样的是 D(temp)以\t判断，这个以\r判断

            //注释里面出现include的话会出事囧
            while(findSubStrAtPos(str,"#include",pos)){}//连续读取代码中的include名
            while(findSubStrAtPos(str,"class ",pos)){}//连续读取代码中的类
            while(findSubStrAtPos(str,"template",pos)){}//连续读取代码中的类

            findGlobalVarsAndFunctions(str);

            display(srcFile);
            fin.close();
        }
    }
    else if(stringUtil.EndWith(srcFile.fileName, ".c"))
    {
        ifstream fin(srcFile.filePath.c_str());//文件输入流，p是代码路径
        string str;
        string temp;
        size_t pos = 0;
        while(getline(fin,temp,'\n'))
        {
            str.append(temp+"\r\n\t");
        }
        R(str);//删除全部注释，跟D(temp)不一样的是 D(temp)以\t判断，这个以\r判断

        //注释里面出现include的话会出事囧
        while(findSubStrAtPos(str,"#include",pos)){}//连续读取代码中的include名
        while(findSubStrAtPos(str,"class ",pos)){}//连续读取代码中的类
        while(findSubStrAtPos(str,"template",pos)){}//连续读取代码中的类

        findGlobalVarsAndFunctions(str);

        display(srcFile);
        fin.close();
    }
    else
    {
        //其他文件格式
    }

    return 0;
}

int CppParser::judge(string s)
{
    if(s=="#include")
    {
        return INCLUDE;
    }
    else if(s=="class ")
    {
        return CLASS;
    }
    else if(s=="template")
    {
        return TEMPLATE_CLASS;
    }
    else if(s=="vaiabler")
    {
        return VARIABLE;
    }
    else if(s=="function")
    {
        return FUNCTION;
    }
    return UNDEINED;
}

void CppParser::D(string& str,char c)
{
    if(c == '#') //如果字符是 '=' ， 则把 = 等号 和 ; 分号之间的信息除去，例如：int a = 5; 把=号、空格和5给擦除
    {
        size_t index_s = 0;
        while(index_s<str.length())
        {
            index_s = str.find(c,index_s);//找 '#' 的位置
            if(index_s != string::npos)
            {
                str.erase(index_s);
            }
        }
        return;
    }
    
    //除'='号以外的字符找到直接删除
    size_t index = 0;
    while(index<str.length())
    {
        index = str.find(c,index);
        if(index != string::npos)
        {
            str.erase(index,1);
        }
    }
}

//除去str中的所有s字符串
void CppParser::D(string& str,string s)
{
    size_t index;
    do{
        index = str.find(s);
        if(index != string::npos)
        {
            str.replace(index,s.length(),"");
        }
        else break;
    }while(1);
}

void CppParser::R(string& str)
{
    size_t index;

    do
    {
        index = str.find("://");// 找到 :// 的位置 各种协议中包含的字符串, 防止和//注释混淆
        if(index != string::npos)
        {
            str.replace(index,3,"");
        }
        else
        {
            break;
        }
    }while(1);

    while(1)
    {
        if(str.find("//")!=string::npos)
        {
            size_t pos = str.find("//");
            size_t end = str.find("\n",pos);//从pos位置开始寻找
            size_t len = end - pos;
            str.erase(pos,len);//删除pos位置开始后长度为len的字符串
        }
        else if(str.find("/*")!=string::npos)
        {
            size_t pos = str.find("/*");
            size_t end = str.find("*/",pos);
            string temp("*/");
            size_t len = end - pos + temp.length();
            str.erase(pos,len);
        }
        else
        {
            break;
        }
    }

    
    do
    {
        index = str.find(", ");//消除,后面的空格
        if(index != string::npos)
        {
            str.replace(index,2,",");
        }
        else
        {
            break;
        }
    }while(1);

    findGlobalClassDeclares(str);
}

//要求是用制表符进行首行推进，而不是用四个空格。
//根据制表符来分解一个string为一个vector容器
vector<string> CppParser::divideByTab(string &str)
{
    vector<string> vs;
    size_t index_s = 0;
    size_t index_e;
    index_s = str.find('\t',index_s);
    if(index_s != string::npos)
    {
        index_e = str.find('\t',index_s+1);
        if(index_e != string::npos)
        {
            do
            {
                if(index_s+1 != index_e)
                {
                    vs.push_back(str.substr(index_s+1 , index_e - index_s-1));
                }
                index_s = index_e;
                index_e = str.find('\t',index_e+1);
            }while(index_e != string::npos);
        }
    }
    if(index_s < str.length())
    {
        vs.push_back(str.substr(index_s+1,str.length() - index_s - 1));
    }
    return vs;
}


//忽略空格和制表符
void CppParser::ignorespacetab(const string& str,size_t &fI)
{
    while(fI<str.length() && (str[fI] == ' ' || str[fI] == '\t'))
    {
        ++fI;
    }
}

//忽略字母和数字
void CppParser::ignorealnum(const string&str ,size_t& fI)
{
    while(fI<str.length() && isalnum(str[fI]))
    {
        ++fI;
    }
}

inline string& CppParser::ltrim(string &str)
{
    string::iterator p = find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(isspace)));
    str.erase(str.begin(), p);
    return str;
}

inline string& CppParser::rtrim(string &str)
{
    string::reverse_iterator p = find_if(str.rbegin(), str.rend(), not1(ptr_fun<int , int>(isspace)));
    str.erase(p.base(), str.end());
    return str;
}

inline string& CppParser::trim(string &str)
{
    ltrim(rtrim(str));
    return str;
}

void CppParser::display(SrcFileModel fileModel)
{
    DataBase *database = DataBase::Instance();

    vector<string>::iterator b;
    size_t pos;
    for(b = include.begin(); b!=include.end();++b)
    {
        //include
    }
    vector<CppParser>::iterator i;
    for(i = _classes.begin(); i!=_classes.end() ; ++i)
    {
        //类名
        string classname = i->classname;
        if(i->extends.size() != 0)
        {
            for(b = i->extends.begin(); b != i->extends.end(); ++b)
            {
                pos = 0;
                trim(*b);
                ignorespacetab(*b,pos);
                if(pos != b->length())
                {
                    //继承
                }
            }
        }
        for(b = i->var.begin(); b != i->var.end(); ++b)
        {
            pos = 0;
            trim(*b);
            ignorespacetab(*b,pos);
            if(pos != b->length())
            {
                string varName = b->substr(pos,b->length()-pos);
                ClassModel model;
                model.fileName = fileModel.fileName;
                model.className = classname;
                model.identifyName = varName;
                model.filePath = fileModel.filePath;

                database->insertRecord(model);
            }
        }
        for(b = i->function.begin(); b != i->function.end(); ++b)
        {
            pos = 0;
            trim(*b);
            ignorespacetab(*b,pos);
            if(pos != b->length())
            {
                string functionName = b->substr(pos,b->length()-pos);
                ClassModel model;
                model.fileName = fileModel.fileName;
                model.className = classname;
                model.identifyName = functionName;
                model.filePath = fileModel.filePath;

                database->insertRecord(model);
            }
        }
    }
}

int CppParser::findGlobalClassDeclares(string& str)
{
    CppParser tempC;
    tempC.classname = "GlobalClassDeclare";
    vector<string> vs = divideByTab(str);//根据制表符分解字符串
    size_t sem_index;//分号下标
    //根据分号来区分函数和变量
    for(vector<string>::iterator it = vs.begin(); it!=vs.end();++it)
    {
        string it_str = trim(*it);
        sem_index = it_str.find_last_of(';');
        if( sem_index != string::npos)
        {
            string cur_var = it_str.substr(0,sem_index);
            cur_var = trim(cur_var);
            size_t cls_index = cur_var.find("class ");

            size_t friend_index = cur_var.find("friend ");
            if (cls_index != string::npos && friend_index != string::npos)
            {
                cls_index = friend_index;
            }

            if (cur_var.length() > 2 && cls_index != string::npos)
            {
                string class_declare_str = cur_var + ";";
                size_t index = str.find(class_declare_str);

                tempC.var.push_back(cur_var);

                if (index != string::npos)
                {
                    str.replace(index, sem_index-cls_index+1, "");
                }
            }
        }
    }

    _classes.push_back(tempC);

    if(tempC.var.size() > 0 || tempC.function.size() > 0)
    {
      return HAVEFOUND;
    }
    else
    {
      return NOTFOUND;
    }
}

int CppParser::findGlobalVarsAndFunctions(string& str)
{
    CppParser tempC;
    tempC.classname = "GlobalVarsAndFunctions";
    size_t lBlock = str.find('{',0) ;// 找{
    size_t cur_index = lBlock;
    vector<size_t> vi = actionscope(str,cur_index);//获取函数和数组变量初始化等 { 和 } 的位置
    string temp = "";
    //排除所有作用域内的字符串
    for(vector<size_t>::iterator vit = vi.begin(); vit != vi.end(); vit += 2)
    {
        size_t start_index = *vit+1;
        size_t substr_index = *(vit+1)-*(vit)-1;

        if (start_index > str.length() || start_index+substr_index > str.length())
        {
            break;
        }

        temp += str.substr(start_index, substr_index);
    }
    vector<string> vs = divideByTab(temp);//根据制表符分解字符串
    size_t sem_index;//分号下标
    //根据分号来区分函数和变量
    for(vector<string>::iterator b = vs.begin(); b!=vs.end();++b)
    {
        sem_index = (int)b->find_last_of(';');
        if( sem_index != string::npos)
        {
            string cur_var = b->substr(0,sem_index);
            cur_var = trim(cur_var);
            if (cur_var.length() > 2)
            {
                tempC.var.push_back(cur_var);
            }
        }
        else
        {
            string cur_function_str = trim(*b);
            if (cur_function_str.length() > 2)
            {
                tempC.function.push_back(cur_function_str);
            }
        }
    }

    _classes.push_back(tempC);

    if(tempC.var.size() > 0 || tempC.function.size() > 0)
    {
        return HAVEFOUND;
    }
    else
    {
        return NOTFOUND;
    }
}

/**
 fI , nI - fI 取得是fI 到 nI-1下标的子串
 */
int CppParser::findSubStrAtPos(string& str,string s,size_t& pos)
{
    int type = judge(s);
    size_t fI,nI;//firstIndex,nextIndex
    string temp = "";
    switch(type)
    {
        case INCLUDE:
        {
            fI = str.find(s,pos);//先找到include的位置
            if(fI != string::npos)
            {
                //判断include是否在class里面的注释里
                size_t cI = str.find("class ",pos);
                if(cI != string::npos && cI<fI)
                {
                    return NOTFOUND;
                }
                fI+=type;//跳过include字符串(含一个空格)
                ignorespacetab(str,fI);//然后忽略剩余的空格或制表符，如果有
                nI = str.find('\n',fI);//找到分号
                temp = str.substr(fI,nI-fI);//include名
                //除去多余的制表符和空格

                if(type == INCLUDE){
                    include.push_back(temp);
                }
                //pos位置为分号右边一位。
                pos = nI + 1;
                return HAVEFOUND;
            }
            else{
                return NOTFOUND;
            }
        }
            break;
        case CLASS:
        {
            fI = str.find(s,pos);//找到"class "
            if(fI != string::npos)
            {
                fI += strlen("class ");
                CppParser theclass;//C类
                size_t lBlock = str.find('{',fI) ;// 找{
                if(lBlock != string::npos)
                {
                    ++lBlock;
                    string classline = str.substr(fI,lBlock-fI);//获得类信息的第一行
                    size_t begin = 0;
                    const string cn = findClassName(classline,begin);//classname
                    vector<string> en = findExtendsName(classline,begin);//extendsname

                    string classname = cn;
                    if (trim(classname).length() <= 0)
                    {
                        return NOTFOUND;
                    }

                    theclass.classname = cn;
                    theclass.extends = en;

                    size_t cur_index = lBlock;//current_index
                    vector<size_t> vi = actionscope(str,cur_index);//获取函数和数组变量初始化等 { 和 } 的位置
                    string temp = "";
                    //排除所有作用域内的字符串
                    for(vector<size_t>::iterator vit = vi.begin(); vit != vi.end(); vit += 2)
                    {
                        size_t start_index = *vit+1;
                        size_t substr_index = *(vit+1)-*(vit)-1;

                        if (start_index > str.length() || start_index+substr_index > str.length())
                        {
                            break;
                        }

                        temp += str.substr(start_index, substr_index);
                    }
                    vector<string> vs = divideByTab(temp);//根据制表符分解字符串
                    size_t sem_index;//分号下标
                    //根据分号来区分函数和变量
                    for(vector<string>::iterator b = vs.begin(); b!=vs.end();++b)
                    {
                        sem_index = b->find_last_of(';');
                        if( sem_index != string::npos)
                        {
                            string cur_var = b->substr(0,sem_index);
                            cur_var = trim(cur_var);
                            if (cur_var.length() > 2)
                            {
                                theclass.var.push_back(cur_var);
                            }
                        }
                        else
                        {
                            string cur_function_str = trim(*b);
                            if (cur_function_str.length() > 2 )
                            {
                                theclass.function.push_back(cur_function_str);
                            }
                        }
                    }

                    _classes.push_back(theclass);
                    pos = fI + 1;//下一个搜索位置从fI开始，因为可能会出现类里面嵌套类的情况
                    return HAVEFOUND;
                }
            }
            else
            {
                return NOTFOUND;
            }
        }
        case TEMPLATE_CLASS:
        {
            fI = str.find(s,pos);//找到"template"
            if(fI != string::npos)
            {
                fI += strlen("template");
                CppParser theclass;//C类
                size_t lBlock = str.find('>',fI) ;// 找template<>的>结束符
                if(lBlock != string::npos)
                {
                    ++lBlock;
                    string classline = str.substr(fI,lBlock-fI);//获得类信息的第一行
                    size_t begin = 0;
                    const string cn = findClassName(classline,begin);//classname
                    vector<string>  en = findExtendsName(classline,begin);//extendsname

                    theclass.classname = cn;
                    theclass.extends = en;

                    size_t cur_index = lBlock;//current_index
                    vector<size_t> vi = actionscope(str,cur_index);//获取函数和数组变量初始化等 { 和 } 的位置
                    string temp = "";
                    //排除所有作用域内的字符串
                    for(vector<size_t>::iterator vit = vi.begin(); vit != vi.end(); vit += 2)
                    {
                        size_t start_index = *vit+1;
                        size_t substr_index = *(vit+1)-*(vit)-1;

                        if (start_index > str.length() || start_index+substr_index > str.length())
                        {
                            break;
                        }

                        temp += str.substr(start_index, substr_index);
                    }
                    vector<string> vs = divideByTab(temp);//根据制表符分解字符串
                    size_t sem_index;//分号下标
                    //根据分号来区分函数和变量
                    for(vector<string>::iterator b = vs.begin(); b!=vs.end();++b)
                    {
                        sem_index = b->find_last_of(';');
                        if( sem_index != string::npos)
                        {
                            string cur_var = b->substr(0,sem_index);
                            cur_var = trim(cur_var);
                            if (cur_var.length() > 2)
                            {
                                theclass.var.push_back(cur_var);
                            }
                        }
                        else
                        {
                            string cur_function_str = trim(*b);
                            if (cur_function_str.length() > 2)
                            {
                                theclass.function.push_back(cur_function_str);
                            }
                        }
                    }

                    size_t template_class_begin = cn.find("<", pos);
                    size_t template_class_end = cn.find(">", pos);
                    if (template_class_begin!=string::npos || template_class_end!=string::npos)
                    {
                        //do nothing
                    }
                    else
                    {
                        _classes.push_back(theclass);
                    }
                    pos = fI + 1;//下一个搜索位置从fI开始，因为可能会出现类里面嵌套类的情况
                    return HAVEFOUND;
                }
            }
            else
            {
                return NOTFOUND;
            }
        }
            break;
        case VARIABLE:
            break;
        case FUNCTION:
            break;
        case UNDEINED:
            break;
    };
    return NOTFOUND;
}

bool CppParser::is_str_contain_space(string str)
{
    return str.find(' ') != string::npos;
}

int CppParser::findFunctionAndVarsOfClass(string& str,string s,size_t& pos,CppParser& theclass)
{
    size_t fI;//firstIndex
    string temp = "";
    
    fI = str.find(s+"::",pos);//找到具体类
    size_t lBlock = str.find("{",fI) ;// 找{
    if(fI != string::npos)
    {
        fI += strlen(s.c_str());
        
        size_t cur_index = lBlock;//current_index
        vector<size_t> vi = actionscope(str,cur_index);//获取函数和数组变量初始化等 { 和 } 的位置
        string temp = "";
        //排除所有作用域内的字符串
        for(vector<size_t>::iterator vit = vi.begin(); vit != vi.end(); vit += 2)
        {
            size_t start_index = *vit+1;
            size_t substr_index = *(vit+1)-*(vit)-1;
            
            if (start_index > str.length() || start_index+substr_index > str.length())
            {
                break;
            }
            
            temp += str.substr(start_index, substr_index);
        }
        D(temp,'#');//删除 # 号 和 \n 号之间的信息，包括#号，不包括\n号
        vector<string> vs = divideByTab(temp);//根据制表符分解字符串
        for(vector<string>::iterator b = vs.begin(); b!=vs.end();++b)
        {
            string cur_function_str = trim(*b);
            if (cur_function_str.length() > 2 && cur_function_str.find(s+"::") != string::npos && is_str_contain_space(cur_function_str))
            {
                theclass.function.push_back(cur_function_str);
            }
        }
        
        sort(theclass.function.begin(),theclass.function.end());
        theclass.function.erase(unique(theclass.function.begin(), theclass.function.end()), theclass.function.end());
        
        pos = fI + 1;//下一个搜索位置从fI开始，因为可能会出现类里面嵌套类的情况
        return HAVEFOUND;
    }
    else
    {
        return NOTFOUND;
    }
}

string CppParser::findClassName(const string& classline,size_t &begin)
{
    string classname = classline;

    size_t space_start = classname.find("  ", 0);//找修饰符的位置
    while(space_start != string::npos)
    {
        classname = classname.replace(space_start, 2, "");
        space_start = classname.find("  ", 0);//找修饰符的位置
    }

    size_t newline_start = classname.find("\r\n\t", 0);//找修饰符的位置
    while(newline_start != string::npos)
    {
        classname = classname.replace(newline_start, 3, "");
        newline_start = classname.find("\r\n\t", 0);//找修饰符的位置
    }

    size_t decorate_start = classname.find("public", 0);//找修饰符的位置
    string result = classname;
    while (decorate_start != string::npos)
    {
        size_t nest_class_index = classname.find("public:class");
        size_t block_index = classname.find("{");
        if (nest_class_index != string::npos)//内部类
        {
            string nest_classname = classname.substr(nest_class_index+strlen("public:class"), block_index-nest_class_index-strlen("public:class"));
            nest_classname = trim(nest_classname);
            return nest_classname;
        }

        result = classname.replace(decorate_start, decorate_start+strlen("public"), "");
        decorate_start = result.find("public", 0);
    }

    decorate_start = classname.find("protected", 0);//找修饰符的位置
    while (decorate_start != string::npos)
    {
        size_t nest_class_index = classname.find("protected:class");
        size_t block_index = classname.find("{");
        if (nest_class_index != string::npos)//内部类
        {
            string nest_classname = classname.substr(nest_class_index+strlen("protected:class"), block_index-nest_class_index-strlen("protected:class"));
            nest_classname = trim(nest_classname);
            return nest_classname;
        }

        result = classname.replace(decorate_start, decorate_start+strlen("protected"), "");
        decorate_start = result.find("protected", 0);
    }

    size_t extends_colon_index = result.find(":");
    size_t not_extends_colon_index = result.find("::");
    string curr_classline = result.substr(0, extends_colon_index);
    curr_classline = trim(curr_classline);
    if (extends_colon_index != string::npos && not_extends_colon_index == string::npos)
    {
        size_t space_index = curr_classline.find_last_of(' ');
        string result_classname = curr_classline;
        if (space_index != string::npos)
        {
            curr_classline = curr_classline.substr(space_index, curr_classline.length()-space_index);
            result_classname = trim(curr_classline);
        }
        else
        {
            result_classname = curr_classline.substr(0, extends_colon_index);
            result_classname = trim(curr_classline);
        }

        return result_classname;
    }

    size_t space_index = curr_classline.find_last_of(' ');
    size_t block_index = curr_classline.find_last_of('{');
    if (space_index != string::npos && block_index != string::npos)
    {
        string result_classname = curr_classline.substr(space_index, block_index-space_index);
        result_classname = trim(result_classname);
        return result_classname;
    }

    ignorespacetab(curr_classline,begin);
    size_t CNS = begin;//classname_start
    ignorealnum(curr_classline,begin);
    size_t CNE = begin;//classname_end

    curr_classline = curr_classline.substr(CNS,CNE-CNS);

    return curr_classline;
}

vector<string> CppParser::split(std::string str,std::string pattern)
{
    string::size_type pos;
    vector<string> result;
    str+=pattern;//扩展字符串以方便操作
    size_t size=str.length();

    for(size_t i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            std::string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

vector<string> CppParser::findExtendsName(const string& str,size_t pos)
{
    vector<string> extends_name;

    string curr_str = str;

    size_t extends_start = curr_str.find(":",pos);//extends_start
    size_t extends_end = curr_str.find("\r\n\t",pos);//extends_end
    if( extends_start != string::npos && extends_end != string::npos)
    {
        string result = curr_str.substr(extends_start+1, extends_end-extends_start-1);

        vector<string> extends = split(result, ",");

        for (vector<string>::iterator str_iter = extends.begin(); str_iter != extends.end(); str_iter++)
        {
            string curr_extend_str = *str_iter;
            size_t public_start = curr_extend_str.find("public", 0);//找修饰符的位置
            size_t protected_start = curr_extend_str.find("protected", 0);//找修饰符的位置
            size_t nest_class_index = curr_extend_str.find("class ", 0);
            if (public_start != string::npos && nest_class_index == string::npos)
            {
                curr_extend_str = curr_extend_str.replace(public_start, strlen("public"), "");
                extends_name.push_back(trim(curr_extend_str));
            }
            else if (protected_start != string::npos && nest_class_index == string::npos)
            {
                curr_extend_str = curr_extend_str.replace(public_start, strlen("protected"), "");
                extends_name.push_back(trim(curr_extend_str));
            }
        }
        return extends_name;
    }
    return extends_name;
}

//以下是判断作用域的位置，并返回合适的 { 和 } 位置来将变量名和函数名分割出来。
void CppParser::actionscope_ignore(const string& str,size_t& fI)
{
    int lBlock_num = 1;
    while(lBlock_num)
    {
        if(fI >= str.length())
        {
            break;
        }
        ++fI;
        if(str[fI] == '{')
        {
            ++lBlock_num;
        }
        else if(str[fI] == '}')
        {
            --lBlock_num;
        }
    }
}


vector<size_t> CppParser::actionscope(const string& str,size_t& fI)
{
    vector<size_t> index;
    index.push_back(fI-1);
    int lBlock_num = 1;
    while(lBlock_num)
    {
        if(fI >= str.length())
        {
            break;
        }
        if(str[fI] == '{')
        {
            index.push_back(fI);//获取'{'的下标
            actionscope_ignore(str,fI);
            index.push_back(fI);//获得匹配上面的'{'的'}'的下标
        }
        else if(str[fI] == '}')
        {
            lBlock_num = 0;
            index.push_back(fI);
            continue;
        }
        ++fI;
    }
    return index;
}
