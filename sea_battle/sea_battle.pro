#-------------------------------------------------
#
# Project created by QtCreator 2019-04-10T18:21:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sea_battle
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ship.cpp \
    battletable.cpp

HEADERS  += mainwindow.h \
    ship.h \
    battletable.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
