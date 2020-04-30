#include "mainwindow.h"
#include "database.h"

#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString file_storage_default_name = "cached_files.dat";
    QString dir_storage_default_name = "cached_dirs.dat";

    if( argc == 3 )
    {
        // setting the custom names of storage files
        file_storage_default_name = QString( argv[1] );
        dir_storage_default_name = QString( argv[2] );
    }

    MainWindow mainWin(file_storage_default_name,dir_storage_default_name);
    mainWin.setMinimumSize(800, 500);
    mainWin.show();

    return app.exec();
}
