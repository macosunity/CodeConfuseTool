#include "dialog.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QCoreApplication>
#include <QTextStream>
#include <QTextEdit>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <vector>

#include "stringutil.h"

#include "cppparser.h"
#include "ocparser.h"
#include "database.h"
#include "resultdialog.h"
#include "garbagecode.h"

#define random(a,b) (rand()%(b-a+1)+a)

#define DEBUG

using namespace std;

void Dialog::readFileList(const char *basePath)
{
    QDir dir(basePath);
    
    if (!dir.exists())
    {
        qDebug() << "dir " << basePath << " not exist!" << endl;
        return;
    }
    
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    
    int file_count = list.count();
    if(file_count <= 0)
    {
        return;
    }
    
    QStringList string_list;
    
    //获取所选文件类型过滤器
    QStringList filters;
    filters << QString("*.*");
    
    //定义迭代器并设置过滤器
    QDirIterator dirIter(basePath,
                         filters,
                         QDir::Files | QDir::NoSymLinks,
                         QDirIterator::Subdirectories);
    
    while(dirIter.hasNext())
    {
        dirIter.next();
        QFileInfo fileInfo = dirIter.fileInfo();
        QString absFilePath = fileInfo.absoluteFilePath();
        qDebug() << absFilePath << endl;
        
        SrcFileModel fileModel;
        fileModel.fileName = fileInfo.fileName().toStdString();
        fileModel.filePath = absFilePath.toStdString();
        fileModel.isParsed = false;
        fileList.push_back(fileModel);
    }
}

Dialog::Dialog(QString file_storage, 
               QString dir_storage, 
               QWidget *parent)
    : QDialog(parent)
{
    this->file_storage = file_storage;
    this->dir_storage = dir_storage;
    
    list = new QListWidget();
    choose = new QPushButton("请选择项目文件夹");
    start = new QPushButton("开始混淆");
    edit_line = new QLineEdit();
    edit_ignore_folders = new QLineEdit();
    lbl_ignore_folders = new QLabel("请输入要忽略的文件夹名字，以空格分割：");
    cb_isconfuse_objc = new QCheckBox("是否混淆Objective C代码", this);
    cb_isconfuse_cpp = new QCheckBox("是否混淆C/C++代码", this);
    cb_isinject_garbagecode_cpp = new QCheckBox("是否在C/C++函数中插入垃圾代码", this);
    
    cb_isconfuse_objc->setCheckState(Qt::Checked);
    //set default flags
    is_confuse_objc = true;
    is_confuse_cpp = false;
    is_inject_garbagecode_cpp = false;
    
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    edit_line->setMinimumWidth(300);
    lbl_ignore_folders->setAlignment(Qt::AlignLeft);
    lbl_ignore_folders->adjustSize();
    choose->setMinimumWidth(140);

    connect(choose, SIGNAL(clicked(bool)), SLOT(choose_path()));
    connect(start, SIGNAL(clicked(bool)), SLOT(start_choosing()));
    connect(cb_isconfuse_objc, SIGNAL(stateChanged(int)), this, SLOT(onConfuseObjCStateChanged(int)));
    connect(cb_isconfuse_cpp, SIGNAL(stateChanged(int)), this, SLOT(onConfuseCppStateChanged(int)));
    connect(cb_isinject_garbagecode_cpp, SIGNAL(stateChanged(int)), this, SLOT(onInjectCodeStateChanged(int)));
    
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(edit_line);
    inputLayout->addWidget(choose);
    
    QVBoxLayout *checksLayout = new QVBoxLayout();
    
    QHBoxLayout *ignoreFolderLayout = new QHBoxLayout();
    ignoreFolderLayout->addWidget(lbl_ignore_folders);
    ignoreFolderLayout->addWidget(edit_ignore_folders);
    ignoreFolderLayout->setStretchFactor(lbl_ignore_folders, 1);
    ignoreFolderLayout->setStretchFactor(edit_ignore_folders, 2);
    
    checksLayout->addLayout(ignoreFolderLayout);
    checksLayout->addWidget(cb_isconfuse_objc);
    checksLayout->addWidget(cb_isconfuse_cpp);
    checksLayout->addWidget(cb_isinject_garbagecode_cpp);
    
    QVBoxLayout *allLayout = new QVBoxLayout();
    allLayout->addLayout(inputLayout);
    allLayout->addLayout(checksLayout);
    allLayout->addWidget(list);
    allLayout->addWidget(start);
    
    setLayout(allLayout);
    setWindowTitle("项目代码混淆工具");
    setWindowFlags(Qt::Window);
    setGeometry(400,400,800,490);
}

void Dialog::choose_path()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), ".");
    edit_line->setText(path);
}

void Dialog::add_next_path(QString path)
{
    list->insertItem(0, path);
}

bool Dialog::is_identify_class(string identify_str)
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


bool Dialog::is_identify_property(string identify_str)
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

void Dialog::onConfuseObjCStateChanged(int state)
{
    if (state == Qt::Checked) // "选中"
    {
        is_confuse_objc = true;
        qDebug() << "选中: " << state << endl;
    }
    else if(state == Qt::PartiallyChecked) // "半选"
    {
        qDebug() << "半选: " << state << endl;
    }
    else // 未选中 - Qt::Unchecked
    {
        is_confuse_objc = false;
        qDebug() << "未选中: " << state << endl;
    }
}

void Dialog::onConfuseCppStateChanged(int state)
{
    if (state == Qt::Checked) // "选中"
    {
        is_confuse_cpp = true;
        qDebug() << "选中: " << state << endl;
    }
    else if(state == Qt::PartiallyChecked) // "半选"
    {
        qDebug() << "半选: " << state << endl;
    }
    else // 未选中 - Qt::Unchecked
    {
        is_confuse_cpp = false;
        qDebug() << "未选中: " << state << endl;
    }
}

void Dialog::onInjectCodeStateChanged(int state)
{
    if (state == Qt::Checked) // "选中"
    {
        is_inject_garbagecode_cpp = true;
        qDebug() << "选中: " << state << endl;
    }
    else if(state == Qt::PartiallyChecked) // "半选"
    {
        qDebug() << "半选: " << state << endl;
    }
    else // 未选中 - Qt::Unchecked
    {
        is_inject_garbagecode_cpp = false;
        qDebug() << "未选中: " << state << endl;
    }
}

void Dialog::inject_garbagecode()
{
    StringUtil stringUtil;
    
    
#ifdef DEBUG
    QDir curr_dir;
    QString jsonPath = curr_dir.absolutePath();
    QDir jsonDir(jsonPath);
    jsonDir.cdUp();
    jsonPath = jsonDir.absolutePath();
    qDebug() << jsonPath;
    jsonPath = jsonPath.append("/garbagepiece.json");
    
    QFile jsonFile(jsonPath);
    if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "critical", "读取垃圾代码文件出错！", QMessageBox::Yes, QMessageBox::Yes);
    }
#else
    QString jsonPath = QCoreApplication::applicationDirPath();
    jsonPath = jsonPath.append("/garbagepiece.json");
    qDebug() << jsonPath;
    QFile jsonFile(jsonPath);
    if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "critical", "读取垃圾代码文件出错！", QMessageBox::Yes, QMessageBox::Yes);
    }
#endif
    
    QString val = jsonFile.readAll();
    qDebug() << val << endl;
    jsonFile.close();
    
    vector<string> garbageCodeArr;
    for (int i=0; i<=6; i++) {
        
        QString indexKey = QString(std::to_string(i).c_str());
        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject jsonObj = doc.object();
        QJsonValue value = jsonObj.value(indexKey);
        
        qDebug() << value << endl;
        garbageCodeArr.push_back(value.toString().toStdString());
    }
    
    QString ignore_folders = edit_ignore_folders->text();
    QStringList ignore_folders_list = ignore_folders.split(" ");
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for(size_t i=0; i<fileList.size(); i++)
    {
        size_t index = i % 6;
        SrcFileModel file = fileList.at(i);
        
        QString QfilePath = QString(file.filePath.c_str());
        //过滤掉已经插入过垃圾代码的cpp文件，还有Pods文件
        if (file.isParsed || QfilePath.contains("Pods/") || QfilePath.contains("sqlite"))
        {
            continue;
        }
        
        if (ignore_folders.length() > 0)
        {
            bool bPathContainsIgnoreFolder = false;
            for (int k=0; k<ignore_folders_list.size(); k++)
            {
                if (QfilePath.contains(ignore_folders_list[k]))
                {
                    bPathContainsIgnoreFolder = true;
                }
            }
            
            if (bPathContainsIgnoreFolder)
            {
                continue;
            }
        }
        
        if(stringUtil.EndWith(file.fileName, ".cpp") || stringUtil.EndWith(file.fileName, ".cxx") || stringUtil.EndWith(file.fileName, ".cc"))
        {
            file.cppFileName = file.fileName;
            file.cppFilePath = file.filePath;
            
            QString cppFilePathString = QString("正在插入垃圾指令:");
            cppFilePathString = cppFilePathString.append(file.cppFilePath.c_str());
            list->addItem(cppFilePathString);
            //qDebug() << cppFilePathString << endl;
            
            GarbageCodeTool gct;
            gct.insertGarbageCode(file, garbageCodeArr[index]);
        }
        else if(stringUtil.EndWith(file.fileName, ".c"))
        {
            findHeaderFileWithFileModel(file);
            
            file.cFileName = file.fileName;
            file.cFilePath = file.filePath;
            
            QString parseInfoString = QString("正在插入垃圾指令:");
            parseInfoString = parseInfoString.append(file.filePath.c_str());
            qDebug() << parseInfoString << endl;
            list->addItem(parseInfoString);
            
            GarbageCodeTool gct;
            gct.insertGarbageCode(file, garbageCodeArr[index]);
        }
        else
        {
            //跳过其他文件
        }
        
        list->update();
        list->repaint();
        list->scrollToBottom();
        QCoreApplication::processEvents();
    }
    QApplication::restoreOverrideCursor();
}

void Dialog::generate_confuse_code()
{
    StringUtil stringUtil;
    
    QString ignore_folders = edit_ignore_folders->text();
    QStringList ignore_folders_list = ignore_folders.split(" ");
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for(size_t i=0; i<fileList.size(); i++)
    {
        SrcFileModel file = fileList.at(i);
        
        QString QfilePath = QString(file.filePath.c_str());
        //过滤掉已经解析过的文件，还有Pods文件
        if (file.isParsed || QfilePath.contains("Pods/") || QfilePath.contains("sqlite"))
        {
            continue;
        }
        
        if (ignore_folders.length() > 0)
        {
            bool bPathContainsIgnoreFolder = false;
            for (int k=0; k<ignore_folders_list.size(); k++)
            {
                if (QfilePath.contains(ignore_folders_list[k]))
                {
                    bPathContainsIgnoreFolder = true;
                }
            }
            
            if (bPathContainsIgnoreFolder)
            {
                continue;
            }
        }

        if(stringUtil.EndWith(file.fileName, ".h") || stringUtil.EndWith(file.fileName, ".hpp"))
        {
            file.headerFilePath = file.filePath;
            
            findCppFileWithFileModel(file);
            findMMFileWithFileModel(file);
            findMFileWithFileModel(file);
            if (is_confuse_cpp && file.cppFilePath.length() > 0)
            {
                QString headFileString = QString("正在分析:");
                headFileString = headFileString.append(file.headerFilePath.c_str());
                list->addItem(headFileString);
                
                QString cppFilePathString = QString("正在分析:");
                cppFilePathString = cppFilePathString.append(file.cppFilePath.c_str());
                list->addItem(cppFilePathString);
                
                CppParser cppParser;
                cppParser.parseCppFile(file);
            }
            
            if(is_confuse_objc && file.mmFilePath.length() > 0)
            {
                QString headFileString = QString("正在分析:");
                headFileString = headFileString.append(file.headerFilePath.c_str());
                list->addItem(headFileString);
                
                QString mFilePathString = QString("正在分析:");
                mFilePathString = mFilePathString.append(file.mmFilePath.c_str());
                list->addItem(mFilePathString);
                
                OCParser ocParser;
                ocParser.parseOCFile(file);
                
                CppParser cppParser;
                cppParser.parseCppFile(file);
                
                SrcFileModel xibfile;
                xibfile.fileName = file.mmFileName;
                xibfile.filePath = file.mmFilePath;
                
                xibAndsb.push_back(xibfile);
            }
            
            if(is_confuse_objc && file.mFilePath.length() > 0)
            {
                QString headFileString = QString("正在分析:");
                headFileString = headFileString.append(file.headerFilePath.c_str());
                list->addItem(headFileString);
                
                QString mFilePathString = QString("正在分析:");
                mFilePathString = mFilePathString.append(file.mFilePath.c_str());
                list->addItem(mFilePathString);
                
                OCParser ocParser;
                ocParser.parseOCFile(file);
                
                SrcFileModel xibfile;
                xibfile.fileName = file.mFileName;
                xibfile.filePath = file.mFilePath;
                
                xibAndsb.push_back(xibfile);
            }
        }
        else if(is_confuse_cpp && (stringUtil.EndWith(file.fileName, ".cpp") || stringUtil.EndWith(file.fileName, ".cxx") || stringUtil.EndWith(file.fileName, ".cc")))
        {
            file.cppFileName = file.fileName;
            file.cppFilePath = file.filePath;
            
            if(findHeaderFileWithFileModel(file))
            {
                QString headerFilePathString = QString("正在分析:");
                headerFilePathString = headerFilePathString.append(file.headerFilePath.c_str());
                list->addItem(headerFilePathString);
            }
            QString cppFilePathString = QString("正在分析:");
            cppFilePathString = cppFilePathString.append(file.cppFilePath.c_str());
            list->addItem(cppFilePathString);
            //qDebug() <<"curr cpp file is:" << cppFilePathString << endl;
            
            CppParser cppParser;
            cppParser.parseCppFile(file);
        }
        else if(is_confuse_cpp && stringUtil.EndWith(file.fileName, ".c"))
        {
            findHeaderFileWithFileModel(file);
            
            file.cFileName = file.fileName;
            file.cFilePath = file.filePath;
            
            QString parseInfoString = QString("正在分析:");
            parseInfoString = parseInfoString.append(file.filePath.c_str());
            list->addItem(parseInfoString);
            
            CppParser cppParser;
            cppParser.parseCppFile(file);
        }
        else if(is_confuse_objc && stringUtil.EndWith(file.fileName, ".m"))
        {
            file.mFileName = file.fileName;
            file.mFilePath = file.filePath;
            
            if(findHeaderFileWithFileModel(file))
            {
                QString headerFilePathString = QString("正在分析:");
                headerFilePathString = headerFilePathString.append(file.headerFilePath.c_str());
                list->addItem(headerFilePathString);
            }
            
            QString parseInfoString = QString("正在分析:");
            parseInfoString = parseInfoString.append(file.filePath.c_str());
            list->addItem(parseInfoString);
            
            OCParser ocParser;
            ocParser.parseOCFile(file);
            
            
            SrcFileModel xibfile;
            xibfile.fileName = file.fileName;
            xibfile.filePath = file.filePath;
            
            xibAndsb.push_back(xibfile);
        }
        else if(is_confuse_objc && stringUtil.EndWith(file.fileName, ".mm"))
        {
            findHeaderFileWithFileModel(file);
            
            file.mmFileName = file.fileName;
            file.mmFilePath = file.filePath;
            
            QString parseInfoString = QString("正在分析:");
            parseInfoString = parseInfoString.append(file.filePath.c_str());
            list->addItem(parseInfoString);
            
            OCParser ocParser;
            ocParser.parseOCFile(file);
            
            CppParser cppParser;
            cppParser.parseCppFile(file);
        }
        else
        {
            //跳过其他文件
        }
        
        list->update();
        list->repaint();
        list->scrollToBottom();
        QCoreApplication::processEvents();
    }
    QApplication::restoreOverrideCursor();
}

void Dialog::start_choosing()
{
    start->setEnabled(false);

    list->setMouseTracking(false);

    list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    list->setSelectionRectVisible(true);
    list->setSelectionBehavior(QAbstractItemView::SelectColumns);
    list->setSelectionMode(QAbstractItemView::NoSelection);
    list->setDragEnabled(false);

    while(list->count() != 0)
    {
        QListWidgetItem *item = list->takeItem(0);
        list->removeItemWidget(item);
        delete item;
    }
    
    DataBase *database = DataBase::Instance();
    database->clearIdentifyVec();
    vector<SrcFileModel>().swap(fileList);

    QString path = edit_line->text();

    QByteArray ba = path.toUtf8();
    char *pathStr = ba.data();
    readFileList(pathStr);
    
    if (is_inject_garbagecode_cpp)
    {
        inject_garbagecode();
    }
    
    if (is_confuse_cpp || is_confuse_objc)
    {
        generate_confuse_code();
    }
    
    
    vector<string> identifyVec = database->queryAll();
    
    vector<string> keysVec;
    putAllKeyWords(keysVec);
    
    vector<string> intersectVec(20000);
    set_intersection(identifyVec.begin(), identifyVec.end(), keysVec.begin(), keysVec.end(), intersectVec.begin());//交集
    sort(intersectVec.begin(),intersectVec.end());
    intersectVec.erase(unique(intersectVec.begin(), intersectVec.end()), intersectVec.end());

    vector<string> resultVec(100000);
    set_difference(identifyVec.begin(), identifyVec.end(), intersectVec.begin(), intersectVec.end(), resultVec.begin()); //差集
    sort(resultVec.begin(),resultVec.end());
    resultVec.erase(unique(resultVec.begin(), resultVec.end()), resultVec.end());


#ifdef DEBUG
    QDir dir;
    QString resPath = dir.absolutePath();
    QDir resDir(resPath);
    resDir.cdUp();
    resPath = resDir.absolutePath();
    qDebug() << resPath;
    resPath = resPath.append("/DICT.txt");
    QFile resFile(resPath);
    if(!resFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "critical", "读取字典文件出错！", QMessageBox::Yes, QMessageBox::Yes);
    }
#else
    QDir dir;
    QString resPath = QCoreApplication::applicationDirPath();
    resPath = resPath.append("/DICT.txt");
    qDebug() << resPath;
    QFile resFile(resPath);
    if(!resFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "critical", "读取字典文件出错！", QMessageBox::Yes, QMessageBox::Yes);
    }
#endif
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    vector<string> wordsVec;
    
    QTextStream stream(&resFile);
    QString line;
    int n = 1;
    while (!stream.atEnd())
    {
        line = stream.readLine();
        string keyword = line.toStdString();
        wordsVec.push_back(keyword);
        ++n;
        
        QCoreApplication::processEvents();
    }
    QApplication::restoreOverrideCursor();
    
    StringUtil stringUtil;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    srand(static_cast<unsigned>(time(nullptr)));
    unordered_set<string> strset;
    string ss = "";
    while(strset.size() < resultVec.size())
    {
        size_t index = static_cast<size_t>(random(1, static_cast<int>(wordsVec.size()-1)));

        ss = wordsVec[index];
        qDebug() << ss.c_str() << endl;
        string ssFirstCharStr = ss.substr(0,1);
        stringUtil.Toupper(ssFirstCharStr);
        ss = ss.replace(0, 1, ssFirstCharStr);
        strset.insert(ss);
        
        QCoreApplication::processEvents();
    }
    QApplication::restoreOverrideCursor();
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    vector<string> disorderIdentifyVec;
    for (unordered_set<string>::iterator it=strset.begin(); it!=strset.end(); it++)
    {
        disorderIdentifyVec.push_back(*it);
        QCoreApplication::processEvents();
    }
    
    QApplication::restoreOverrideCursor();
    
    //启用list
    list->setEditTriggers(QAbstractItemView::AllEditTriggers);
    list->setSelectionBehavior(QAbstractItemView::SelectItems);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setDragEnabled(true);
    
    ResultDialog *pResultDlg = new ResultDialog(this);
    pResultDlg->setModal(true);
    pResultDlg->setConfuseResult(resultVec, disorderIdentifyVec);
    pResultDlg->show();
    
    start->setEnabled(true);
}

bool Dialog::findCppFileWithFileModel(SrcFileModel &fileModel)
{
    QFile srcFile(fileModel.filePath.c_str());
    QFileInfo fileInfo = QFileInfo(srcFile);

    string cppFileName = fileInfo.baseName().append(".cpp").toStdString();
    string ccFileName = fileInfo.baseName().append(".cc").toStdString();
    string cxxFileName = fileInfo.baseName().append(".cxx").toStdString();

    StringUtil stringUtil;
    for(size_t i=0; i<fileList.size(); i++)
    {
        SrcFileModel file = fileList.at(i);
        string filePath = file.filePath;
        stringUtil.Tolower(filePath);
        stringUtil.Tolower(cppFileName);
        stringUtil.Tolower(ccFileName);
        stringUtil.Tolower(cxxFileName);
        if(stringUtil.EndWith(filePath, "/"+cppFileName) || stringUtil.EndWith(filePath, "/"+ccFileName) || stringUtil.EndWith(filePath, "/"+cxxFileName))
        {
            QFile qfile(file.filePath.c_str());
            QFileInfo fi = QFileInfo(qfile);
            QString fileBaseName = fi.fileName();
            QString cppFilePath = fi.absolutePath();
            cppFilePath = cppFilePath.append("/").append(fileBaseName);
            fileModel.cppFilePath = cppFilePath.toStdString();
            fileModel.cppFileName = fileBaseName.toStdString();

            file.isParsed = true;
            SrcFileModel tempModel = file;
            tempModel.isParsed = true;
            fileList[i] = tempModel;
            return true;
        }
    }

    return false;
}

//Objective C
bool Dialog::findMFileWithFileModel(SrcFileModel &fileModel)
{
    QFile srcFile(fileModel.filePath.c_str());
    QFileInfo fileInfo = QFileInfo(srcFile);

    string ocFileName = fileInfo.baseName().append(".m").toStdString();

    StringUtil stringUtil;
    for(size_t i=0; i<fileList.size(); i++)
    {
        SrcFileModel file = fileList.at(i);
        if(stringUtil.EndWith(file.filePath, "/"+ocFileName))
        {
            QFile qfile(file.filePath.c_str());
            QFileInfo fi = QFileInfo(qfile);
            QString fileBaseName = fi.baseName();
            fileBaseName = fileBaseName.append(".m");
            QString mFilePath = fi.absolutePath();
            mFilePath = mFilePath.append("/").append(fileBaseName);
            fileModel.mFilePath = mFilePath.toStdString();
            fileModel.mFileName = fileBaseName.toStdString();

            file.isParsed = true;
            SrcFileModel tempModel = file;
            tempModel.isParsed = true;
            fileList[i] = tempModel;
            return true;
        }
    }

    return false;
}

//Objective C++
bool Dialog::findMMFileWithFileModel(SrcFileModel &fileModel)
{
    QFile srcFile(fileModel.filePath.c_str());
    QFileInfo fileInfo = QFileInfo(srcFile);

    string ocFileName = fileInfo.baseName().append(".mm").toStdString();

    StringUtil stringUtil;
    for(size_t i=0; i<fileList.size(); i++)
    {
        SrcFileModel file = fileList.at(i);
        if(stringUtil.EndWith(file.filePath, "/"+ocFileName))
        {
            QFile qfile(file.filePath.c_str());
            QFileInfo fi = QFileInfo(qfile);
            QString fileBaseName = fi.baseName();
            fileBaseName = fileBaseName.append(".mm");
            QString mmFilePath = fi.absolutePath();
            mmFilePath = mmFilePath.append("/").append(fileBaseName);
            fileModel.mmFilePath = mmFilePath.toStdString();
            fileModel.mmFileName = fileBaseName.toStdString();

            file.isParsed = true;
            SrcFileModel tempModel = file;
            tempModel.isParsed = true;
            fileList[i] = tempModel;
            return true;
        }
    }

    return false;
}

bool Dialog::findHeaderFileWithFileModel(SrcFileModel &fileModel)
{
    QFile qSrcfile(fileModel.filePath.c_str());
    QFileInfo fileInfo = QFileInfo(qSrcfile);

    string headerFileName = fileInfo.baseName().append(".h").toStdString();
    StringUtil stringUtil;
    for(size_t i=0; i<fileList.size(); i++)
    {
        SrcFileModel file = fileList.at(i);
        if(stringUtil.EndWith(file.filePath, "/"+headerFileName))
        {
            QFile qfile(file.filePath.c_str());
            QFileInfo fi = QFileInfo(qfile);
            QString fileBaseName = fi.baseName();
            fileBaseName = fileBaseName.append(".h");
            QString headerFilePath = fi.absolutePath();
            headerFilePath = headerFilePath.append("/").append(fileBaseName);
            fileModel.headerFilePath = headerFilePath.toStdString();
            fileModel.headerFileName = fileBaseName.toStdString();

            SrcFileModel tempModel = file;
            tempModel.isParsed = true;
            fileList[i] = tempModel;
            return true;
        }
    }

    return false;
}

Dialog::~Dialog()
{
    
}

void Dialog::putAllKeyWords(vector<string> &keysVec)
{
#ifdef DEBUG
    QDir dir;
    QString resPath = dir.absolutePath();
    QDir resDir(resPath);
    resDir.cdUp();
    resPath = resDir.absolutePath();
    qDebug() << resPath;
    resPath = resPath.append("/reskeys.txt");
    QFile resFile(resPath);
    if(!resFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "critical", "读取关键字文件出错！", QMessageBox::Yes, QMessageBox::Yes);
    }
#else
    QDir dir;
    QString resPath = QCoreApplication::applicationDirPath();
    resPath = resPath.append("/reskeys.txt");
    qDebug() << resPath;
    QFile resFile(resPath);
    if(!resFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(nullptr, "critical", "读取关键字文件出错！", QMessageBox::Yes, QMessageBox::Yes);
    }
#endif
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextStream stream(&resFile);
    QString line;
    int n = 1;
    while (!stream.atEnd())
    {
        line = stream.readLine();
        string keyword = line.toStdString();
        keysVec.push_back(keyword);
        ++n;
        QCoreApplication::processEvents();
    }
    
    QApplication::restoreOverrideCursor();
    
    sort(keysVec.begin(), keysVec.end());
    
    resFile.close();
}
