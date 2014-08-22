#-------------------------------------------------
#
# Project created by QtCreator 2014-08-12T09:53:42
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -O1 -g -I..\qconnect\include\

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qconnect
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    vpninfo.cpp \
    storage.cpp \
    editdialog.cpp \
    keypair.cpp \
    key.cpp \
    cert.cpp \
    logdialog.cpp \
    gtdb.cpp

HEADERS  += mainwindow.h \
    vpninfo.h \
    storage.h \
    editdialog.h \
    common.h \
    keypair.h \
    key.h \
    cert.h \
    logdialog.h \
    gtdb.h

FORMS    += mainwindow.ui \
    editdialog.ui \
    logdialog.ui

unix|win32: LIBS += -L..\qconnect\lib -lopenconnect -lgnutls -lwsock32

RESOURCES += \
    resources.qrc
