#-------------------------------------------------
#
# Project created by QtCreator 2016-06-26T21:10:03
#
#-------------------------------------------------

QT       += printsupport network core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Printer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    downloaddialog.cpp

HEADERS  += mainwindow.h \
    downloaddialog.h

FORMS    += mainwindow.ui
