#-------------------------------------------------
#
# Project created by QtCreator 2014-12-10T11:11:20
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = map
TEMPLATE = app

SOURCES += main.cpp\
        mapmainwindow.cpp \
    aufgabenliste.cpp \
    aufgabe.cpp \
    dialog.cpp \
    mdutils.cpp \
    aboutdlg.cpp \
    helpbrowser.cpp \
    editierbareaufgabe.cpp \
    mdlistwidgetwithdrop.cpp \
    mdplaintexteditwithdrop.cpp

HEADERS  += mapmainwindow.h \
    aufgabenliste.h \
    aufgabe.h \
    dialog.h \
    mdutils.h \
    aboutdlg.h \
    helpbrowser.h \
    editierbareaufgabe.h \
    mdlistwidgetwithdrop.h \
    mdplaintexteditwithdrop.h

FORMS    += mapmainwindow.ui \
    dialog.ui \
    aboutdlg.ui

RESOURCES += \
    mapressources.qrc

RC_FILE = map.rc

VERSION = 5.0.1

