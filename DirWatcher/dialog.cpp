#include "dialog.h"
#include <QDebug>

Dialog::Dialog(QString file_storage, 
               QString dir_storage, 
               QWidget *parent)
    : QDialog(parent)
{
    this->file_storage = file_storage;
    this->dir_storage = dir_storage;
    
    list = new QListWidget();
    add = new QPushButton("Add");
    choose = new QPushButton("Choose directrory");
    start = new QPushButton("Start");
    edit_line = new QLineEdit();
    del = new QPushButton("Delete");
    
    list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    edit_line->setMinimumWidth(300);
    choose->setMinimumWidth(140);
    add->setMinimumWidth(140);
    del->setMinimumWidth(140);
    
    connect(del, SIGNAL(clicked(bool)), SLOT(del_path()));
    connect(choose, SIGNAL(clicked(bool)), SLOT(choose_path()));
    connect(add, SIGNAL(clicked(bool)), SLOT(add_path()));
    connect(start, SIGNAL(clicked(bool)), SLOT(start_choosing()));
    
    QHBoxLayout *input = new QHBoxLayout();
    input->addWidget(edit_line);
    input->addWidget(choose);
    
    QHBoxLayout *for_add = new QHBoxLayout();
    for_add->addStretch(2);
    for_add->addWidget(add);
    
    QHBoxLayout *for_del = new QHBoxLayout();
    for_del->addStretch(2);
    for_del->addWidget(del);
    
    QVBoxLayout *all = new QVBoxLayout();
    all->addLayout(input);
    all->addLayout(for_add);
    all->addLayout(for_del);
    all->addWidget(list);
    all->addWidget(start);
    
    setLayout(all);
    setWindowTitle("Directory chooser");
    setWindowFlags(Qt::Window);
    setGeometry(400,400,300,400);
}

void Dialog::choose_path()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), ".");
    edit_line->setText(path);
}

void Dialog::add_path()
{
    if( !edit_line->text().isEmpty() )
        list->addItem(edit_line->text());
}

void Dialog::del_path()
{
    QList<QListWidgetItem*> items = list->selectedItems();
    foreach(QListWidgetItem *item , items)
        delete item;
}

void Dialog::add_next_path(QString path)
{
    list->insertItem(0, path);
}

void Dialog::start_choosing()
{   
    if( list->count() == 0 )
        return;
    
    QString item = list->item(0)->text();
    delete list->item(0);
    FileViewer viewer(item, file_storage, dir_storage);
    connect(&viewer, SIGNAL(new_path(QString)), SLOT(add_next_path(QString)) );
    viewer.start();
    viewer.exec();
}

Dialog::~Dialog()
{
    
}
