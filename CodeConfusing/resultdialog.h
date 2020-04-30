#pragma once

#include <QDialog>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <vector>
#include <string>
#include "srcfilemodel.h"

using namespace std;

class ResultDialog : public QDialog
{
    Q_OBJECT
protected:
    QPushButton *ok_btn;
    QTextEdit *edit_result;
    
public:
    ResultDialog(QWidget *parent = nullptr);
    ~ResultDialog();
    void setConfuseResult(vector<string> resultVec, vector<string> disorderIdentifyVec);
    
private slots:
    void hideDialog();
};
