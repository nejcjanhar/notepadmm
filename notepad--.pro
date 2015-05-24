#-------------------------------------------------
#
# Project created by QtCreator 2015-04-26T19:35:22
#
#-------------------------------------------------

QT       += core gui\
           printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = notepad--
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    settings.cpp \
    multitabedit.cpp \
    simplecrypt.cpp

HEADERS  += mainwindow.h \
    settings.h \
    multitabedit.h \
    simplecrypt.h

FORMS    += mainwindow.ui \
    settings.ui

RC_FILE += Icon.rc

RESOURCES += \
    style.qrc

DISTFILES +=

