#include "ocparser.h"
#include "stringutil.h"
#include "database.h"

OCParser::OCParser()
{
    extends = "";
    classname = "";
}

OCParser::OCParser(string extends)
{
    classname = "";
    this->extends = extends;
}

int OCParser::parseOCFile(SrcFileModel srcFile)
{
    QFile file(srcFile.filePath.c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    StringUtil stringUtil;
    if(stringUtil.EndWith(srcFile.fileName, ".h"))
    {
        ifstream fin(srcFile.filePath.c_str());//文件输入流
        string str;
        string temp;
        size_t pos = 0;
        while(getline(fin,temp,'\n'))
        {
            str.append(temp+"\r\n\t");
        }
        R(str);//删除全部注释，跟D(temp)不一样的是 D(temp)以\t判断，这个以\r判断
        //注释里面出现include和import的话会出事囧
        while(find(str,"#include",pos)){}//连续读取代码中的include名
        while(find(str,"#import",pos)){}//连续读取代码中的import名
        while(find(str,"@interface",pos)){}//连续读取代码中的类
        while(find(str,"@implementation",pos)){}//连续读取代码中的类

        QFile mFile(srcFile.mFilePath.c_str());
        if(mFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            fin.close();

            ifstream m_fin(srcFile.mFilePath.c_str());//文件输入流，p是代码路径
            string m_str;
            string m_temp;
            size_t m_pos = 0;
            while(getline(m_fin,m_temp,'\n'))
            {
                m_str.append(m_temp+"\r\n\t");
            }
            R(m_str);//删除全部注释，跟D(temp)不一样的是 D(temp)以\t判断，这个以\r判断
            srcFile.fileImpleParseString = m_str;
            //注释里面出现include和import的话会出事囧
            while(find(m_str,"#include",m_pos)){}//连续读取代码中的include名
            while(find(m_str,"#import",m_pos)){}//连续读取代码中的import名
            while(find(m_str,"@interface",m_pos)){}//连续读取代码中的类
            while(find(m_str,"@implementation",m_pos)){}//连续读取代码中的类

            display(srcFile);
            m_fin.close();
        }
        else
        {
            display(srcFile);
            fin.close();
        }
    }
    else if(stringUtil.EndWith(srcFile.fileName, ".m") || stringUtil.EndWith(srcFile.fileName, ".mm"))
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
        srcFile.fileImpleParseString = str;
        //注释里面出现include和import的话会出事囧
        while(find(str,"#include",pos)){}//连续读取代码中的include名
        while(find(str,"#import",pos)){}//连续读取代码中的import名
        while(find(str,"@interface",pos)){}//连续读取代码中的类
        while(find(str,"@implementation",pos)){}//连续读取代码中的类

        QFile headFile(srcFile.headerFilePath.c_str());
        if(headFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            fin.close();

            ifstream header_fin(srcFile.headerFilePath.c_str());//文件输入流，p是代码路径
            string header_str;
            string header_temp;
            size_t header_pos = 0;
            while(getline(header_fin,header_temp,'\n'))
            {
                header_str.append(header_temp+"\r\n\t");
            }
            R(header_str);//删除全部注释，跟D(temp)不一样的是 D(temp)以\t判断，这个以\r判断
            //注释里面出现include和import的话会出事囧
            while(find(header_str,"#include",header_pos)){}//连续读取代码中的include名
            while(find(header_str,"#import",header_pos)){}//连续读取代码中的import名
            while(find(header_str,"@interface",header_pos)){}//连续读取代码中的类
            while(find(header_str,"@implementation",header_pos)){}//连续读取代码中的类

            display(srcFile);
            header_fin.close();
        }
        else
        {
            display(srcFile);

            fin.close();
        }
    }
    else
    {
        //其他文件格式
    }

    return 0;
}

int OCParser::judge(string s)
{
    if(s=="#include")
    {
        return OC_INCLUDE;
    }
    else if(s=="#import")
    {
        return OC_IMPORT;
    }
    else if(s=="@interface")
    {
        return OC_CLASS;
    }
    else if(s=="@implementation")
    {
        return OC_IMPLEMENTS;
    }
    else if(s=="vaiabler")
    {
        return OC_VARIABLE;
    }
    else if(s=="function")
    {
        return OC_IMPORT;
    }
    return OC_UNDEINED;
}

void OCParser::D(string& str,char c)
{
    if(c == '=') //如果字符是 '=' ， 则把 = 等号 和 ; 分号之间的信息除去，例如：int a = 5; 把=号、空格和5给擦除
    {
        size_t index_s = 0;
        size_t index_e = 0;
        while(index_s<str.length())
        {
            index_s = str.find(c,index_s);//找 '=' 的位置
            if(index_s != string::npos)
            {
                index_e = str.find(';',index_s+1);//找 ';' 的位置
                if(index_e != string::npos)
                {
                    str.replace(index_s,index_e-index_s,"");//擦除
                    index_s = index_e;
                }
                else
                {
                    str.erase(index_s);
                }
            }
        }
        return;
    }
    else if(c == ' ')
    {
        size_t index = 0;
        while(index<str.length())
        {
            index = str.find(c,index);
            if(index != string::npos)
            {
                size_t fI = index;
                //如果import的库里面有多个空格，那么仅仅保留一个
                while(index<str.length() && str[++index] == ' ')
                {
                }
                if(index - 1 != fI)
                {
                    str.erase(fI,index - fI - 1);
                }
            }
        }
        return;
    }
    else if(c == '#') //如果字符是 '=' ， 则把 = 等号 和 ; 分号之间的信息除去，例如：int a = 5; 把=号、空格和5给擦除
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

void OCParser::R(string& str)
{
    size_t index;
    do
    {
        index = str.find("://");// 找到 :// 的位置 各种协议中包含的字符串, 防止和//注释混淆
        if(index != string::npos){
            size_t index_quote = str.find("\"",index+1);//找到 双引号" 的位置
            if(index_quote != string::npos)
            {
                str.replace(index,index_quote + 1 - index,"");
            }
            else if(index<str.length())
            {
                str.replace(index,str.length()-index + 1,"");
            }
            else
            {
                break;
            }
        }
        else break;
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
}

//要求是用制表符进行首行推进，而不是用四个空格。
//根据制表符来分解一个string为一个vector容器
vector<string> OCParser::divideByTab(string &str)
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
void OCParser::ignorespacetab(const string& str,size_t &fI)
{
    while(fI<str.length() && (str[fI] == ' ' || str[fI] == '\t'))
    {
        ++fI;
    }
}

//忽略字母和数字
void OCParser::ignorealnum(const string&str,size_t& fI)
{
    while(fI<str.length() && isalnum(str[fI]))
    {
        ++fI;
    }
}

inline string& OCParser::ltrim(string &str)
{
    string::iterator p = find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(isspace)));
    str.erase(str.begin(), p);
    return str;
}

inline string& OCParser::rtrim(string &str)
{
    string::reverse_iterator p = find_if(str.rbegin(), str.rend(), not1(ptr_fun<int , int>(isspace)));
    str.erase(p.base(), str.end());
    return str;
}

inline string& OCParser::trim(string &str)
{
    ltrim(rtrim(str));
    return str;
}

inline bool OCParser::is_property_name_exist(string functionName, vector<string> propertyList)
{
    StringUtil stringUtil;
    for(size_t i=0; i<propertyList.size(); i++)
    {
        if(stringUtil.StartWith(propertyList[i], functionName) && propertyList[i].length() == functionName.length())
        {
            return true;
        }
    }
    return false;
}

void OCParser::display(SrcFileModel fileModel)
{
    DataBase *database = DataBase::Instance();

    vector<string>::iterator b;
    size_t pos;
    for(b = oc_include.begin(); b!=oc_include.end();++b)
    {
        //include
    }
    for(b = oc_import.begin(); b!=oc_import.end();++b)
    {
        //import
    }
    vector<OCParser>::iterator i;
    for(i = _oc.begin(); i!=_oc.end() ; ++i)
    {
        //类名
        string oc_class_name = i->classname;
        if(i->extends.size() != 0)
        {
            //继承
        }
        if(i->delegates.size()!=0)
        {
            for(b = i->delegates.begin(); b != i->delegates.end(); ++b)
            {
                //代理
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
                //不混淆xib绑定的属性和方法
                if (varName.find("IBOutlet") != string::npos || varName.find("IBAction") != string::npos)
                {
                    continue;
                }
                
                ClassModel model;
                model.fileName = fileModel.fileName;
                model.className = oc_class_name;
                model.identifyName = varName;
                model.identifyOriginName = varName;
                model.filePath = fileModel.filePath;
                model.isObjectiveC = true;
                
                if (handleObjectiveCIdentify(model))
                {
                    database->insertRecord(model);
                }
            }
        }
        for(b = i->properties.begin(); b != i->properties.end(); ++b)
        {
            pos = 0;
            trim(*b);
            ignorespacetab(*b,pos);
            if (pos != b->length())
            {
                string propertyName = b->substr(pos,b->length()-pos);
                
                //不混淆xib绑定的属性
                if (propertyName.find("IBOutlet") != string::npos)
                {
                    continue;
                }
                
                ClassModel model;
                model.fileName = fileModel.fileName;
                model.className = oc_class_name;
                model.identifyName = propertyName;
                model.identifyOriginName = propertyName;
                model.filePath = fileModel.filePath;
                model.isObjectiveC = true;
                
                if (handleObjectiveCIdentify(model))
                {
                    database->insertRecord(model);
                }

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
                
                //不混淆xib绑定的方法
                if (functionName.find("IBAction") != string::npos)
                {
                    continue;
                }
                
                if(!is_property_name_exist(functionName, i->properties))
                {
                    ClassModel model;
                    model.fileName = fileModel.fileName;
                    model.className = oc_class_name;
                    model.identifyName = functionName;
                    model.identifyOriginName = functionName;
                    model.filePath = fileModel.filePath;
                    model.isObjectiveC = true;

                    if (handleObjectiveCIdentify(model))
                    {
                        database->insertRecord(model);
                    }
                }
            }
        }
    }
}

/**
 fI , nI - fI 取得是fI 到 nI-1下标的子串
 */
int OCParser::find(string& str,string s,size_t& pos){
    int type = judge(s);
    size_t fI,nI;//firstIndex,nextIndex
    string temp = "";
    switch(type)
    {
        case OC_INCLUDE:
        case OC_IMPORT:
        {
            fI = str.find(s,pos);//先找到include或import的位置
            if(fI != string::npos)
            {
                //判断include和import是否在@interface里面的注释里
                size_t cI = str.find("@interface",pos);
                if(cI != string::npos && cI<fI)
                {
                    return OC_NOTFOUND;
                }
                fI+=type;//跳过include 或import 两个字符串(含一个空格)
                ignorespacetab(str,fI);//然后忽略剩余的空格或制表符，如果有
                nI = str.find('\n',fI);//找到分号
                temp = str.substr(fI,nI-fI);//include名或import名
                //除去多余的制表符和空格
                D(temp,'\t');
                D(temp,' ');
                if(type == OC_INCLUDE){
                    oc_include.push_back(temp);
                }
                else if(type == 7){
                    oc_import.push_back(temp);
                }
                //pos位置为分号右边一位。
                pos = nI + 1;
                return OC_HAVEFOUND;
            }
            else{
                return OC_NOTFOUND;
            }
        }
        case OC_CLASS:
        {
            fI = str.find(s,pos);//找到"@interface"
            if(fI != string::npos)
            {
                fI += strlen("@interface")+1;
                OCParser theclass;//C类
                size_t lBlock = str.find("@end",fI) ;// 找{
                if(lBlock != string::npos)
                {
                    ++lBlock;
                    string classline = str.substr(fI,lBlock-fI);//获得类信息的第一行
                    size_t begin = 0;
                    const string cn = findClassName(classline,begin);//classname
                    const string en = findExtendsName(classline,begin);//extendsname
                    const vector<string> dn = findDelegatesName(classline,begin);//delegatesname
                    map<string, vector<string>> map = findPropertiesAndFunctionDeclaresName(classline,begin);//propertiesname

                    theclass.classname = cn;
                    theclass.extends = en;
                    theclass.delegates = dn;
                    theclass.properties = map["properties"];
                    theclass.function = map["functions"];

                    _oc.push_back(theclass);
                    pos = fI + 1;//下一个搜索位置从fI开始，因为可能会出现类里面嵌套类的情况
                    return OC_HAVEFOUND;
                }
            }
            else
            {
                return OC_NOTFOUND;
            }
        }
            break;
        case OC_IMPLEMENTS:
        {
            fI = str.find(s,pos);//找到"@implementation"
            if(fI != string::npos)
            {
                fI += strlen("@implementation")+1;
                OCParser theclass;//C类
                size_t lBlock = str.find('\n',fI) ;
                if(lBlock != string::npos)
                {
                    ++lBlock;
                    string classline = str.substr(fI,lBlock-fI);//获得类信息的第一行
                    size_t begin = 0;
                    const string cn = findClassName(classline,begin);//classname
                    theclass.classname = cn;

                    size_t cur_index = lBlock;//current_index
                    vector<size_t> vi = actionscope(str,cur_index);//获取函数和数组变量初始化等 { 和 } 的位置
                    string temp = "";
                    //排除所有作用域内的字符串
                    for(vector<size_t>::iterator vit = vi.begin(); vit != vi.end(); vit += 2)
                    {
                        size_t index = *vit+1;
                        size_t length = *(vit+1)-*(vit)-1;
                        if (index >= str.length() || index+length >= str.length())
                        {
                            break;
                        }
                        temp += str.substr(index, length);
                    }
                    vector<string> vs = divideByTab(temp);//根据制表符分解字符串
                    size_t sem_index;//分号下标
                    //根据分号来区分函数和变量
                    for(vector<string>::iterator b = vs.begin(); b!=vs.end();++b)
                    {
                        sem_index = b->find_last_of(';');
                        if( sem_index != string::npos)
                        {
                            theclass.var.push_back(b->substr(0,sem_index));
                        }
                        else
                        {
                            string functionStr = trim(*b);
                            D(functionStr,'#');
                            if (functionStr.length() > 2 && functionStr.find("@") == string::npos)
                            {
                                theclass.function.push_back(*b);
                            }
                        }
                    }
                    sort(theclass.var.begin(),theclass.var.end());
                    theclass.var.erase(unique(theclass.var.begin(), theclass.var.end()), theclass.var.end());
                    sort(theclass.function.begin(),theclass.function.end());
                    theclass.function.erase(unique(theclass.function.begin(), theclass.function.end()), theclass.function.end());
                    _oc.push_back(theclass);
                    pos = fI + 1;//下一个搜索位置从fI开始，因为可能会出现类里面嵌套类的情况
                    return OC_HAVEFOUND;
                }
            }
            else
            {
                return OC_NOTFOUND;
            }
        }
            break;
        case OC_VARIABLE:
            break;
        case OC_FUNCTION:
            break;
        case OC_UNDEINED:
            break;
    };
    return OC_NOTFOUND;
}

string OCParser::findClassName(const string& classline,size_t &begin)
{
    ignorespacetab(classline,begin);
    size_t CNS = begin;//classname_start
    ignorealnum(classline,begin);
    size_t CNE = begin;//classname_end
    return classline.substr(CNS,CNE-CNS);
}

string OCParser::findExtendsName(const string& str,size_t pos)
{
    size_t es = str.find(":",pos);//extends_start
    if( es != string::npos )
    {
        es += 1;
        ignorespacetab(str,es);
        size_t ens = es;//extendsname_start
        ignorealnum(str,es);
        size_t ene = es;//extendsname_end;

        return str.substr(ens,ene-ens);
    }
    return "";
}

const vector<string> OCParser::findDelegatesName(const string& str,size_t pos)
{
    vector<string> delegates;
    size_t ds = str.find("<",pos);//delegates_start
    size_t de = str.find(">",pos);//delegates_end
    if( ds != string::npos)
    {
        ds += 1;
        ignorespacetab(str,ds);
        size_t ins = ds;//delegates_start
        size_t ine = de;//delegates_end

        delegates.push_back(str.substr(ins,ine-ins));
    }
    return delegates;
}

const map<string,vector<string>> OCParser::findPropertiesAndFunctionDeclaresName(const string& str,size_t pos)
{
    map<string,vector<string>> propertiesAndFunctionDeclaresMap;
    vector<string> properties;
    vector<string> functionDeclares;
    size_t ps = str.find("@property",pos);//property_start
    size_t pe = str.find(";",pos);//property_end
    if( ps != string::npos)
    {
        ignorespacetab(str,ps);
        size_t ins = ps;//property_start
        size_t ine = pe;//property_end

        while(ps<str.length())
        {
            if(str[ps] == ' ' || str[ps] == '\t')
            {
                ++ps;
                continue;
            }
            if(str[ps] == ';')
            {
                string judgestr = str.substr(ins,ine-ins);
                D(judgestr, '#');
                judgestr = trim(judgestr);

                size_t propertyPos = judgestr.find("@property");
                size_t endPos = judgestr.find("@end");
                size_t instanceMethodPos = judgestr.find("-");
                size_t classMethodPos = judgestr.find("+");

                if (propertyPos != string::npos && endPos == string::npos)
                {
                    properties.push_back(judgestr);
                    ins = ++ps;
                    continue;
                }
                if (instanceMethodPos != string::npos && endPos == string::npos)
                {
                    functionDeclares.push_back(judgestr);
                    ins = ++ps;
                    continue;
                }
                if (classMethodPos != string::npos && endPos == string::npos)
                {
                    functionDeclares.push_back(judgestr);
                    ins = ++ps;
                    continue;
                }
            }
            ine = ++ps;
        }

        propertiesAndFunctionDeclaresMap.insert(STRING2VECTOR::value_type("properties", properties));
        propertiesAndFunctionDeclaresMap.insert(STRING2VECTOR::value_type("functions", functionDeclares));
    }
    return propertiesAndFunctionDeclaresMap;
}

//以下是判断作用域的位置，并返回合适的 { 和 } 位置来将变量名和函数名分割出来。
void OCParser::actionscope_ignore(const string& str,size_t& fI)
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

vector<size_t> OCParser::actionscope(const string& str,size_t& fI)
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


bool OCParser::handleObjectiveCIdentify(ClassModel &classModel)
{
    StringUtil stringUtil;
    
    string identify_str = classModel.identifyName;
    
    size_t NS1 = identify_str.find("NS_AVAILABLE_IOS");
    if (NS1 != string::npos)
    {
        identify_str = identify_str.substr(0, NS1);
    }
    
    size_t ATTR = identify_str.find("__attribute__");
    if (ATTR != string::npos)
    {
        identify_str = identify_str.substr(0, ATTR);
    }
    
    size_t UI1 = identify_str.find("UI_APPEARANCE_SELECTOR");
    if (UI1 != string::npos)
    {
        identify_str = identify_str.substr(0, UI1);
    }
    
    identify_str = trim(identify_str);
    
    size_t operator_index = identify_str.find(" operator");
    size_t operator_index2 = identify_str.find("::operator");
    size_t method_index = identify_str.find('+');
    size_t method_index2 = identify_str.find('-');
    
    size_t property_index = identify_str.find("@property");
    if ( (method_index != string::npos || method_index2 != string::npos) &&
        (operator_index==string::npos && operator_index2==string::npos) )//Objective C Method
    {
        if (stringUtil.StartWith(identify_str, "set"))
        {
            return false;
        }
        
        size_t first_colon_index = identify_str.find_first_of(':');
        if (first_colon_index != string::npos)
        {
            identify_str = identify_str.substr(0, first_colon_index);
        }
        
        size_t first_brackets_index = identify_str.find_last_of(')');
        if (first_brackets_index != string::npos)
        {
            identify_str = identify_str.substr(first_brackets_index+1, identify_str.length()-first_brackets_index);
        }
        
        stringUtil.deleteSpecialChar(identify_str);
        if (stringUtil.is_allow_identify_name(identify_str))
        {
            string method_str = trim(identify_str);
            if (stringUtil.is_allow_identify_name(classModel.className))
            {
                classModel.identifyName = method_str;
                classModel.isMethodName = true;
                
                return true;
            }
        }
    }
    else if(property_index != string::npos)//Objective C Property
    {
        size_t block_index = identify_str.find_first_of('^');
        if (block_index != string::npos)
        {
            identify_str = identify_str.substr(block_index+1, identify_str.length()-block_index);
            size_t first_brackets_index = identify_str.find_first_of(')');
            if (first_brackets_index != string::npos)
            {
                identify_str = identify_str.substr(0, first_brackets_index);
            }
            return false;
        }
        
        size_t last_space_index = identify_str.find_last_of(' ');
        if (last_space_index != string::npos)
        {
            identify_str = identify_str.substr(last_space_index, identify_str.length()-last_space_index);
        }
        
        stringUtil.deleteSpecialChar(identify_str);
        if (stringUtil.is_allow_identify_name(identify_str))
        {
            string property_str = trim(identify_str);
            if (stringUtil.is_allow_identify_name(classModel.className))
            {
                classModel.identifyName = property_str;
                classModel.isPropertyName = true;
                return true;
            }
        }
    }
    else
    {
        size_t first_brackets_index = identify_str.find_first_of('(');
        if (first_brackets_index != string::npos)
        {
            identify_str = identify_str.substr(0, first_brackets_index);
        }
        
        size_t last_space_index = identify_str.find_last_of(' ');
        if (last_space_index != string::npos)
        {
            identify_str = identify_str.substr(last_space_index, identify_str.length()-last_space_index);
        }
        
        stringUtil.deleteSpecialChar(identify_str);
        if (stringUtil.is_allow_identify_name(identify_str))
        {
            //qDebug() << "other identify:" << classModel.identifyName.c_str() << identify_str.c_str();
        }
    }
    
    return false;
}
