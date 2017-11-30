#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QCheckBox>
#include <vector>
#include <string>
#include <unordered_set>
#include "fileviewer.h"
#include "srcfilemodel.h"

using namespace std;

class Dialog : public QDialog
{
    Q_OBJECT
private:
    QListWidget *list;
    QPushButton *add;
    QPushButton *start;
    QPushButton *choose;
    QCheckBox *cb_isconfuse_objc;
    QLineEdit *edit_line;
    QPushButton *del;
    
    QString file_storage;
    QString dir_storage;
    vector<SrcFileModel> fileList;
    vector<SrcFileModel> xibAndsb;
    
    bool is_confuse_objc;
    
public:
    Dialog(QString file_storage, QString dir_storage, QWidget *parent = 0);
    
    ~Dialog();

public slots:
    void add_next_path(QString path);
    
private slots:
    void putAllKeyWords(vector<string> &keysVec);
    bool findMFileWithFileModel(SrcFileModel &fileModel);
    bool findMMFileWithFileModel(SrcFileModel &fileModel);
    bool findCppFileWithFileModel(SrcFileModel &fileModel);
    bool findHeaderFileWithFileModel(SrcFileModel &fileModel);
    void readFileList(const char *basePath);
    void choose_path();
    void start_choosing();
    void onStateChanged(int state);
    
    bool is_identify_property(string identify_str);
    bool is_identify_class(string identify_str);
};

#endif // DIALOG_H
