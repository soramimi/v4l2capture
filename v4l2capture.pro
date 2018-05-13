#-------------------------------------------------
#
# Project created by QtCreator 2017-12-03T19:08:22
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = Hoge
TEMPLATE = app

LIBS += -lv4lconvert

SOURCES += main.cpp \
    MainWindow.cpp \
    ImageWidget.cpp \
    CameraCaptureThread.cpp

HEADERS  += \
    MainWindow.h \
    ImageWidget.h \
    CameraCaptureThread.h

FORMS    += \
    MainWindow.ui
