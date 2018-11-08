#-------------------------------------------------
#
# Project created by QtCreator 2018-11-08T07:57:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += /opt/openexr-2.2.1/include/OpenEXR

QT += opengl

TARGET = bvhview
TEMPLATE = app

QMAKE_CXXFLAGS += -Wno-deprecated-register

SOURCES += main.cpp\
        mainwindow.cpp \
    myqglwidget.cpp \
    bvh.cpp

HEADERS  += mainwindow.h \
    myapplication.h \
    myqglwidget.h \
    bvh.h \
    dualquaternion.h

FORMS    += mainwindow.ui
