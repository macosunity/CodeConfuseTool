#include "resultdialog.h"

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

void ResultDialog::setConfuseResult(vector<string> resultVec, vector<string> disorderIdentifyVec)
{
    QString resultStr = "";
    for (size_t i=1; i<resultVec.size(); ++i)
    {
        resultStr.append("#ifndef ").append(resultVec[i].c_str()).append("\n");
        resultStr.append("#define ").append(resultVec[i].c_str()).append(" ").append(disorderIdentifyVec[i].c_str()).append("\n");
        resultStr.append("#endif").append("\n");
    }
    
    edit_result->setText(resultStr);
}

void ResultDialog::hideDialog()
{
    this->hide();
}
