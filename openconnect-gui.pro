#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

TARGET = openconnect-gui
TEMPLATE = app

CONFIG += qt
CONFIG += debug_and_release
CONFIG += warn_on
win32:debug {
    CONFIG += console
}

QT += \
    core \
    gui \
    widgets \
    network

SOURCES += \
    main.cpp\
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

HEADERS += \
    mainwindow.h \
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

FORMS += \
    mainwindow.ui \
    editdialog.ui \
    logdialog.ui

RESOURCES += \
    resources.qrc

# we can generate RC file, but we can't generate 'manifest' file on mingw
win32: RC_FILE = openconnect-gui.rc

win32 {
    INCLUDEPATH += Z:/openconnect-gui/include
    LIBS += -LZ:\openconnect-gui\lib -lwsock32
}
unix {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib
}
unix|win32: LIBS += -lopenconnect -lgnutls

