#-------------------------------------------------
#
# Project created by QtCreator 2018-09-12T23:39:53
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = ImgCalibAndCapture
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    exifdata.cpp \
    utilities.cpp \
    ExifTool.cpp \
    ExifToolPipe.cpp \
    TagInfo.cpp

HEADERS += \
        mainwindow.h \
    exifdata.h \
    utilities.h \
    ExifTool.h \
    ExifToolPipe.h \
    TagInfo.h \
    dms.h

FORMS += \
        mainwindow.ui

install_it.path = $$OUT_PWD
install_it.files = ./resources/*

INSTALLS += \
    install_it

INCLUDEPATH +=/usr/local/include/opencv
INCLUDEPATH +=/usr/local/include/opencv2
INCLUDEPATH +=/usr/include/eigen3

LIBS += `pkg-config opencv --cflags --libs` -lboost_date_time

DISTFILES += \
    resources/config.yaml
