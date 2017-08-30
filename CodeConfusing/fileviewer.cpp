#include "fileviewer.h"
#include <QDebug>

FileViewer::FileViewer(QString dir, QString file_path, 
                       QString dir_path, QWidget *parent)
    : QDialog(parent)
{
    this->dir = new QDir(dir);
    this->file_out = new QFile(file_path);
    this->dir_out = new QFile(dir_path);
    
    this->current_file = new QLabel("");
    this->file_name = new QLabel("");
    
    this->file_name->setFont(QFont("Ubuntu", 25));
    
    this->add = new QPushButton("Add");
    this->skip = new QPushButton("Skip");
    this->look_inside = new QPushButton("Look inside");
    
    connect(add, SIGNAL(clicked(bool)), SLOT(add_clicked()));
    connect(skip, SIGNAL(clicked(bool)), SLOT(skip_clicked()));
    connect(look_inside, SIGNAL(clicked(bool)), SLOT(look_inside_clicked()));
    
    add->setEnabled(false);
    skip->setEnabled(false);
    look_inside->setEnabled(false);
    
    set_up_layouts(dir);
    
    file_out->open(QIODevice::WriteOnly | QIODevice::Append);
    dir_out->open(QIODevice::WriteOnly | QIODevice::Append);
    
    setWindowTitle("Chooser");
    setWindowFlags(Qt::Window);
    setGeometry(400,400,500,400);
}

void FileViewer::set_up_layouts(QString dir)
{
    QHBoxLayout *a = new QHBoxLayout();
    a->addWidget(new QLabel("Directory:"));
    a->addWidget(new QLabel(dir));
    
    QHBoxLayout *b = new QHBoxLayout();
    b->addWidget(new QLabel("Current file:"));
    b->addWidget(current_file);
    
    QVBoxLayout *header = new QVBoxLayout();
    header->addLayout(a);
    header->addLayout(b);
    
    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addWidget(look_inside);
    buttons->addWidget(add);
    buttons->addWidget(skip);
    
    QVBoxLayout *all = new QVBoxLayout();
    all->addLayout(header);
    all->addStretch(1);
    all->addWidget(file_name, 0, Qt::AlignCenter);
    all->addStretch(1);
    all->addLayout(buttons);
    
    setLayout(all);
}

void FileViewer::start()
{   
    dir->setSorting(QDir::DirsLast);
    // dir->setFilter(QDir::NoDotAndDotDot);
    this->list = dir->entryInfoList();
    index = -1; // it really matters and needed
    
    next();
    
    this->add->setEnabled(true);
    this->skip->setEnabled(true);
    this->look_inside->setEnabled(true);
}

void FileViewer::next()
{
    index++;
    if( index < list.size() )
    {
        current_file->setText( list[index].absoluteFilePath() );
        file_name->setText( list[index].fileName() );
    }
    else
    {
        this->close();
    }
}

void FileViewer::add_clicked()
{
    QByteArray array;
    QString string = list[index].fileName() + "   #"
                     + list[index].absoluteFilePath() + "\n";
    array.append(string);
    
    if( list[index].isFile() )
    {
        file_out->write( array );
    }
    else if( list[index].isDir() )
    {
        dir_out->write(array);
    }
    
    next();
}


void FileViewer::skip_clicked()
{
    next();
}

void FileViewer::look_inside_clicked()
{
    emit new_path( list[index].absoluteFilePath() );
    next();
}

FileViewer::~FileViewer()
{
    if( file_out->isOpen() ){
        file_out->flush();
        file_out->close();
    }
    if( dir_out->isOpen() ){
        dir_out->flush();
        dir_out->close();
    }
}
