#-------------------------------------------------
#
# Project created by QtCreator 2017-11-09T09:14:04
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++1z

TARGET = TeQtSimpleNotepad

TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    findreplacedialog.cpp \
    settingsutil.cpp

HEADERS  += mainwindow.h \
    findreplacedialog.h \
    settingsuntil.h

FORMS    += mainwindow.ui \
    findreplacedialog.ui

RESOURCES += \
    resources.qrc
