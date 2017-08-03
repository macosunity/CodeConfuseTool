#include "resultdialog.h"
#include "classmodel.h"
#include "database.h"
#include "stringutil.h"

#define random(a,b) (rand()%(b-a+1)+a)

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
        if (stringUtil.StartWith(model.identifyName, identify_str) && model.identifyName.length() == identify_str.length() && model.isObjectiveC)
        {
            return true;
        }
    }
    
    return false;
}

void ResultDialog::setConfuseResult(vector<string> resultVec)
{
    StringUtil stringUtil;
    QString resultStr = "";
    vector<string> finalResultVec;
    for (size_t i=1; i<resultVec.size(); ++i)
    {
        string identify_str = resultVec[i];
        finalResultVec.push_back(identify_str);
        
        if (is_identify_property(resultVec[i]))
        {
            string _property_str = "_" + identify_str;
            finalResultVec.push_back(_property_str);
            
            string firstCharStr = identify_str.substr(0,1);
            stringUtil.Toupper(firstCharStr);
            string upperFirstCaseString = identify_str.replace(0, 1, firstCharStr);
            string set_property_str = "set" + identify_str;
            
            finalResultVec.push_back(set_property_str);
        }
    }
    
    sort(finalResultVec.begin(),finalResultVec.end());
    finalResultVec.erase(unique(finalResultVec.begin(), finalResultVec.end()), finalResultVec.end());
    
    
    const string alpha[] = {"A","B","C", "D", "E","F", "G", "H", "I", "J", "K", "L", "M", "N", "O","P","Q","R","S","T","U","V","W","X","Y","Z",
        "a","b","c", "d", "e","f", "g", "h", "i", "j", "k", "l", "m", "n", "o","p","q","r","s","t","u","v","w","x","y","z","s","t"};
    const string numeric[] = {"0","1","2","3","4","5","6","7","8","9","0","5","3"};
    const string underline = "_";
    
    srand((unsigned)time(NULL));
    unordered_set<string> strset;
    string ss = "";
    while(strset.size() < finalResultVec.size())
    {
        ss = "";
        int alphaLength = 52;
        int numericLength = 11;
        for(int i = 0; i < 22; i++)
        {
            int index = random(1, (alphaLength + numericLength));
            if(index < alphaLength){
                ss.append(alpha[index]);
            } else {
                ss.append(numeric[index - alphaLength]);
            }
        }
        ss = "_"+ss;
        strset.insert(ss);
    }
    
    vector<string> disorderIdentifyVec;
    for (unordered_set<string>::iterator it=strset.begin(); it!=strset.end(); it++)
    {
        disorderIdentifyVec.push_back(*it);
    }
    
    for (size_t i=1; i<finalResultVec.size(); ++i)
    {
        string identify_str = finalResultVec[i];
        
        resultStr.append("#ifndef ").append(identify_str.c_str()).append("\n");
        resultStr.append("#define ").append(identify_str.c_str()).append(" ").append(disorderIdentifyVec[i].c_str()).append("\n");
        resultStr.append("#endif").append("\n");
    }
    
    edit_result->setText(resultStr);
}

void ResultDialog::hideDialog()
{
    this->hide();
}
