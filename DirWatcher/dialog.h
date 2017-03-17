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

#include "fileviewer.h"

class Dialog : public QDialog
{
    Q_OBJECT
private:
    QListWidget *list;
    QPushButton *add;
    QPushButton *start;
    QPushButton *choose;
    QLineEdit *edit_line;
    QPushButton *del;
    
    QString file_storage;
    QString dir_storage;
    
public:
    Dialog(QString file_storage, QString dir_storage, QWidget *parent = 0);
    
    ~Dialog();

public slots:
    void add_next_path(QString path);
    
private slots:
    void choose_path();
    void add_path();
    void del_path();
    void start_choosing();
};

#endif // DIALOG_H
