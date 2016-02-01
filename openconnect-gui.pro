#-------------------------------------------------
#
# Project created by QtCreator 2014-08-12T09:53:42
#
#-------------------------------------------------

QMAKE_CXXFLAGS += -O2 -g
win32: QMAKE_CXXFLAGS += -IZ:\openconnect-gui\include\ 
#unix: QMAKE_CXXFLAGS += -I/usr/local/include

mac: INCLUDEPATH += /usr/local/include

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = openconnect-gui
TEMPLATE = app

win32: RC_FILE = openconnect-gui.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    vpninfo.cpp \
    storage.cpp \
    editdialog.cpp \
    keypair.cpp \
    key.cpp \
    cert.cpp \
    logdialog.cpp \
    gtdb.cpp \
    cryptdata.cpp

HEADERS  += mainwindow.h \
    vpninfo.h \
    storage.h \
    editdialog.h \
    common.h \
    keypair.h \
    key.h \
    cert.h \
    logdialog.h \
    gtdb.h \
    dialogs.h \
    cryptdata.h

FORMS    += mainwindow.ui \
    editdialog.ui \
    logdialog.ui

win32: LIBS += -LZ:\openconnect-gui\lib -lwsock32
unix: LIBS += -L/usr/local/lib
unix|win32: LIBS += -lopenconnect -lgnutls

RESOURCES += \
    resources.qrc

