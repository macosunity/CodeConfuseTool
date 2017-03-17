#include "dialog.h"
#include <QApplication>
#include <QString>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    QString file_storage_default_name = "cached_files.dat";
    QString dir_storage_default_name = "cached_dirs.dat";
    
    if( argc == 3 )
    {
        // setting the custom names of storage files
        file_storage_default_name = QString( argv[1] );
        dir_storage_default_name = QString( argv[2] );
    }
    
    Dialog w( file_storage_default_name ,
              dir_storage_default_name   );
    w.show();
    
    return a.exec();
}
