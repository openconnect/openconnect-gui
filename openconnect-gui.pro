#-------------------------------------------------
#
# Project created by QtCreator 2014-08-12T09:53:42
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -O2 -g
win32: QMAKE_CXXFLAGS += -IZ:\openconnect-gui\include\ 

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = openconnect-gui
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

unix|win32: LIBS += -LZ:\openconnect-gui\lib -lopenconnect -lgnutls
win32: LIBS += -lwsock32

RESOURCES += \
    resources.qrc
