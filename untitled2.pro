#-------------------------------------------------
#
# Project created by QtCreator 2015-04-20T10:34:55
#
#-------------------------------------------------

QT       += core gui
win32: LIBS += -lSetupAPI
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    hid.cpp \
    motor_control.cpp \
    sampleobj.cpp \
    common.cpp

HEADERS  += mainwindow.h \
    hidapi.h \
    motor_control.h \
    sampleobj.h \
    common.h

FORMS    += mainwindow.ui
