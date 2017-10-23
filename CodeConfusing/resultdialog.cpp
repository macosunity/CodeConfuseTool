#include "resultdialog.h"
#include "classmodel.h"
#include "database.h"
#include "stringutil.h"

ResultDialog::ResultDialog(QWidget *parent)
:QDialog(parent)
{
    ok_btn = new QPushButton("好的");
    
    edit_result = new QTextEdit();
    edit_result->setMinimumWidth(300);
    
    QHBoxLayout *input = new QHBoxLayout();
    input->addWidget(edit_result);
    
    QVBoxLayout *all = new QVBoxLayout();
    all->addLayout(input);
    all->addWidget(ok_btn);
    
    connect(ok_btn, SIGNAL(clicked(bool)), SLOT(hideDialog()));

    setLayout(all);
    
    setWindowTitle("混淆结果");
    setWindowFlags(Qt::Window);
    setGeometry(400,400,800,490);
}

ResultDialog::~ResultDialog()
{
    
}

bool is_identify_property(string identify_str)
{
    DataBase *database = DataBase::Instance();
    
    vector<ClassModel> modelVec = database->queryAllModel();
    
    StringUtil stringUtil;
    for (vector<ClassModel>::iterator it=modelVec.begin(); it != modelVec.end(); ++it)
    {
        ClassModel model = *it;
        if (stringUtil.StartWith(model.identifyName, identify_str) && model.identifyName.length() == identify_str.length() && model.isPropertyName && model.identifyName.find("readonly") == string::npos)
        {
            return true;
        }
    }
    
    return false;
}

bool is_identify_class(string identify_str)
{
    DataBase *database = DataBase::Instance();
    
    vector<ClassModel> modelVec = database->queryAllModel();
    
    StringUtil stringUtil;
    for (vector<ClassModel>::iterator it=modelVec.begin(); it != modelVec.end(); ++it)
    {
        ClassModel model = *it;
        if (stringUtil.StartWith(model.className, identify_str) && model.className.length() == identify_str.length())
        {
            return true;
        }
    }
    
    return false;
}

void ResultDialog::setConfuseResult(vector<string> resultVec, vector<string> disorderIdentifyVec, vector<SrcFileModel> xibAndsb)
{
    StringUtil stringUtil;
    QString resultStr = "";
    for (size_t i=1; i<resultVec.size(); ++i)
    {
        string identify_str = resultVec[i];
        
        
        if (is_identify_property(resultVec[i]))
        {
            string id_str = identify_str;
            string res_str = disorderIdentifyVec[i];
            
            for (size_t x=0; x<xibAndsb.size(); x++)
            {
                SrcFileModel file = xibAndsb[x];
                
                string filename = file.fileName;
                
                if (stringUtil.EndWith(filename, ".m") || stringUtil.EndWith(filename, ".mm"))
                {
                    continue;
                }
                
                string sedReplaceStr = "sed -i " + filename + ".bak " + "'s/\"" + id_str +"\"/\"" + res_str +"\"/g' " + file.filePath;
                
                system(sedReplaceStr.c_str());
            }
            
            
            resultStr.append("#define ").append(identify_str.c_str()).append(" ").append(disorderIdentifyVec[i].c_str()).append("\n");

            string _property_str = "_" + identify_str;
            resultStr.append("#define ").append(_property_str.c_str()).append(" _").append(disorderIdentifyVec[i].c_str()).append("\n");
            
            string firstCharStr = identify_str.substr(0,1);
            stringUtil.Toupper(firstCharStr);
            string upperFirstCaseString = identify_str.replace(0, 1, firstCharStr);
            string set_property_str = "set" + upperFirstCaseString;
            
            string redefineString = disorderIdentifyVec[i];
            string redefineFirstCharStr = redefineString.substr(0,1);
            stringUtil.Toupper(redefineFirstCharStr);
            string upperFirstRedefine = redefineString.replace(0, 1, redefineFirstCharStr);
            
            resultStr.append("#define ").append(set_property_str.c_str()).append(" set").append(upperFirstRedefine.c_str()).append("\n");
        }
        else
        {
            string start = "";
            string end = "";
            
            bool isFirstLetterUpper = false;
            char firstLetter = identify_str[0];
            if (isupper(firstLetter))
            {
                isFirstLetterUpper = true;
            }
            
            if (identify_str.length() >= 4)
            {
                start = identify_str.substr(0, 3);
            }
            else
            {
                start = identify_str.substr(0, identify_str.length()-1);
            }
            
            if (identify_str.length() >= 6)
            {
                end = identify_str.substr(identify_str.length()-4, 3);
            }
            else
            {
                string back = identify_str;
                reverse(back.begin(),back.end());
                end = back;
            }
            
            if (isFirstLetterUpper)
            {
                string startFirstCharStr = start.substr(0,1);
                stringUtil.Toupper(startFirstCharStr);
                start = start.replace(0, 1, startFirstCharStr);
            }
            
            resultStr.append("#define ").append(identify_str.c_str()).append(" ").append(start.c_str()).append(disorderIdentifyVec[i].c_str()).append(end.c_str()).append("\n");
            
            if (is_identify_class(identify_str))
            {
                string redefineStr = disorderIdentifyVec[i];
                for (size_t j=0; j<xibAndsb.size(); j++)
                {
                    SrcFileModel file = xibAndsb[j];
                    
                    string nextStr = start+redefineStr+end;
                    
                    string filename = file.fileName;
                    
                    string sedReplaceStr = "sed -i " + filename + ".bak " + "'s/\"" + identify_str +"\"/\"" + nextStr +"\"/g' " + file.filePath;
                    if (stringUtil.EndWith(filename, ".pbxproj"))
                    {
                        sedReplaceStr = "sed -i " + filename + ".bak " + "'s/" + identify_str + ".xib/" + nextStr + ".xib/g' " + file.filePath;
                        
                        system(sedReplaceStr.c_str());
                        continue;
                    }
                    
                    system(sedReplaceStr.c_str());
                }
            }
        }
    }
    
    for (size_t i=1; i<resultVec.size(); ++i)
    {
        string identify_str = resultVec[i];
        
        string start = "";
        string end = "";
        
        bool isFirstLetterUpper = false;
        char firstLetter = identify_str[0];
        if (isupper(firstLetter))
        {
            isFirstLetterUpper = true;
        }
        
        if (identify_str.length() >= 4)
        {
            start = identify_str.substr(0, 3);
        }
        else
        {
            start = identify_str.substr(0, identify_str.length()-1);
        }
        
        if (identify_str.length() >= 6)
        {
            end = identify_str.substr(identify_str.length()-4, 3);
        }
        else
        {
            string back = identify_str;
            reverse(back.begin(),back.end());
            end = back;
        }
        
        if (is_identify_class(identify_str))
        {
            string redefineStr = disorderIdentifyVec[i];
            for (size_t j=0; j<xibAndsb.size(); j++)
            {
                SrcFileModel file = xibAndsb[j];
                
                string nextStr = start+redefineStr+end;
                
                string filename = file.fileName;
                
                if (stringUtil.EndWith(filename, ".xib") && stringUtil.StartWith(filename, identify_str))
                {                    string filenameNew = nextStr + ".xib";
                    string filePathBack = file.filePath;
                    
                    string filePathNew = file.filePath.replace(file.filePath.find(filename), filename.length(), filenameNew);
                    string renameFileStr = "mv " + filePathBack + " " + filePathNew;
                    
                    system(renameFileStr.c_str());
                }
            }
        }
    }
    
    
    edit_result->setText(resultStr);
}

void ResultDialog::hideDialog()
{
    this->hide();
}
