#-------------------------------------------------
#
# Project created by QtCreator 2017-10-27T17:55:46
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ConfAdjustQt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    xmllibrary.cpp \
    wdialogset.cpp

HEADERS  += mainwindow.h \
    xmllibrary.h \
    wdialogset.h

FORMS    += mainwindow.ui \
    wdialogset.ui

RESOURCES += \
    resource.qrc
