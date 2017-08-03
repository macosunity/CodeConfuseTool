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
        if (stringUtil.StartWith(model.identifyName, identify_str) && model.identifyName.length() == identify_str.length() && model.isObjectiveC)
        {
            return true;
        }
    }
    
    return false;
}

void ResultDialog::setConfuseResult(vector<string> resultVec, vector<string> disorderIdentifyVec)
{
    StringUtil stringUtil;
    QString resultStr = "";
    for (size_t i=1; i<resultVec.size(); ++i)
    {
        string identify_str = resultVec[i];
        
        resultStr.append("#ifndef ").append(identify_str.c_str()).append("\n");
        resultStr.append("#define ").append(identify_str.c_str()).append(" ").append(disorderIdentifyVec[i].c_str()).append("\n");
        resultStr.append("#endif").append("\n");
//        
//        if (is_identify_property(resultVec[i]))
//        {
            string _property_str = "_" + identify_str;
            resultStr.append("#ifndef ").append(_property_str.c_str()).append("\n");
            resultStr.append("#define ").append(_property_str.c_str()).append(" _").append(disorderIdentifyVec[i].c_str()).append("\n");
            resultStr.append("#endif").append("\n");
            
            string firstCharStr = identify_str.substr(0,1);
            stringUtil.Toupper(firstCharStr);
            string upperFirstCaseString = identify_str.replace(0, 1, firstCharStr);
            string set_property_str = "set" + identify_str;
            
            resultStr.append("#ifndef ").append(set_property_str.c_str()).append("\n");
            resultStr.append("#define ").append(set_property_str.c_str()).append(" set").append(disorderIdentifyVec[i].c_str()).append("\n");
            resultStr.append("#endif").append("\n");
//        }
    }
    
    edit_result->setText(resultStr);
}

void ResultDialog::hideDialog()
{
    this->hide();
}
