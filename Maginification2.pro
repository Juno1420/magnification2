#-------------------------------------------------
#
# Project created by QtCreator 2016-03-04T12:19:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Maginification2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    poiinfo.cpp \
    tableinfo.cpp \
    labelinfo.cpp \
    bandinfo.cpp \
    graphicsinfo.cpp

HEADERS  += mainwindow.h \
    poiinfo.h \
    tableinfo.h \
    labelinfo.h \
    bandinfo.h \
    graphicsinfo.h \
    VolumeMagnification.h \
    VideoMagnification.h \
    Magnify.h \
    GridMagnification.h \
    GraphMagnification.h

FORMS    += mainwindow.ui

RESOURCES += \
    myImage.qrc

#以下为配置openCV之后增加的内�?

QT       -= gui

CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += C:\opencv\build\include\
                C:\opencv\build\include\opencv\
                C:\opencv\build\include\opencv2\

LIBS += C:\opencv\MinGw\lib\libopencv_core249.dll.a
LIBS += C:\opencv\MinGw\lib\libopencv_highgui249.dll.a
LIBS += C:\opencv\MinGw\lib\libopencv_imgproc249.dll.a
