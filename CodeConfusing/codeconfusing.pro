
QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 link_pkgconfig

TARGET = codeconfusing
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    fileviewer.cpp \
    cppparser.cpp \
    ocparser.cpp \
    srcfilemodel.cpp \
    stringutil.cpp \
    database.cpp \
    classmodel.cpp \
    resultdialog.cpp \
    garbagecode.cpp

HEADERS  += \
    mainwindow.h \
    fileviewer.h \
    cppparser.h \
    ocparser.h \
    srcfilemodel.h \
    stringutil.h \
    database.h \
    classmodel.h \
    resultdialog.h \
    garbagecode.h

RESOURCES += \
    resources.qrc
