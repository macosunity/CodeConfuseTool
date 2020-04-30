#pragma once

#include "fileviewer.h"
#include "srcfilemodel.h"

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
#include <QMainWindow>
#include <QLabel>

#include <vector>
#include <string>
#include <unordered_set>

using namespace std;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QString file_storage, QString dir_storage, QWidget *parent = nullptr);
    
    ~MainWindow();

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
    void onConfuseObjCStateChanged(int state);
    void onConfuseCppStateChanged(int state);
    void onInjectCodeStateChanged(int state);
    
    void generate_confuse_code();
    void inject_garbagecode();
    
    bool is_identify_property(string identify_str);
    bool is_identify_class(string identify_str);

private:
    QListWidget *list = nullptr;
    QPushButton *add = nullptr;
    QPushButton *start = nullptr;
    QPushButton *choose = nullptr;
    QLabel  *lbl_ignore_folders = nullptr;
    QCheckBox *cb_isconfuse_objc = nullptr;
    QCheckBox *cb_isconfuse_cpp = nullptr;
    QCheckBox *cb_isinject_garbagecode_cpp = nullptr;
    QLineEdit *edit_ignore_folders = nullptr;
    QLineEdit *edit_line = nullptr;
    QPushButton *del = nullptr;

    QString file_storage;
    QString dir_storage;
    vector<SrcFileModel> fileList;
    vector<SrcFileModel> xibAndsb;

    bool is_confuse_objc;
    bool is_confuse_cpp;
    bool is_inject_garbagecode_cpp;
};
