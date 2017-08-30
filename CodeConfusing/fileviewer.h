#ifndef FILEVIEWER_H
#define FILEVIEWER_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QDir>
#include <QFileInfo>


class FileViewer : public QDialog
{
    Q_OBJECT
private:
    QLabel *file_name;
    QLabel *current_file;
    
    QPushButton *add;
    QPushButton *skip;
    QPushButton *look_inside;
    
    QFile *file_out;
    QFile *dir_out;
    QDir *dir;
    
    QFileInfoList list;
    int index;
    
    void set_up_layouts(QString dir);
    
public:
    FileViewer(QString dir = "", QString file_path = "", QString dir_path = "", QWidget *parent = 0);
    
    void start();
    void next();
    
    ~FileViewer();
    
signals:
    void new_path(QString path);
    
private slots:
    void add_clicked();
    void skip_clicked();
    void look_inside_clicked();
    
};

#endif // FILEVIEWER_H
