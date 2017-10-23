#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

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
    ResultDialog(QWidget *parent = 0);
    ~ResultDialog();
    void setConfuseResult(vector<string> resultVec, vector<string> disorderIdentifyVec, vector<SrcFileModel> xibAndsb);
    
private slots:
    void hideDialog();
};

#endif // RESULTDIALOG_H
