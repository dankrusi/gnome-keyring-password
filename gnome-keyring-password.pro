#-------------------------------------------------
#
# Project created by QtCreator 2010-10-18T17:41:51
#
#-------------------------------------------------

QT       -= gui

TARGET = gnome-keyring-password
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp


LIBS += -lglib-2.0
LIBS += -lgnome-keyring
INCLUDEPATH += /usr/include/gnome-keyring-1
INCLUDEPATH += /usr/include/glib-2.0
INCLUDEPATH += /usr/lib/glib-2.0/include/
INCLUDEPATH += /usr/include/gnome-keyring-1
INCLUDEPATH += /usr/include/glib-2.0
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include
