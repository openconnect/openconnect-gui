#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
extern "C" {
#include <openconnect.h>
#include <signal.h>
}

int main(int argc, char *argv[])
{
    int ret;
    QApplication a(argc, argv);
    QVariant v;
    MainWindow w;
    QCoreApplication::setOrganizationDomain("redhat.com");

    signal(SIGPIPE, SIG_IGN);
    QSettings settings("Red Hat", "Qconnect");
    openconnect_init_ssl();

#if 0
    v = settings.value("mainwindow/size");
    if (v.isNull() == false)
        w.resize(v.toSize());

    v = settings.value("mainwindow/pos");
    if (v.isNull() == false)
        w.move(v.toPoint());

    v = settings.value("mainwindow/fullscreen");
    if (v.isNull() == false && v.toInt() != 0) {
        w.setWindowState(Qt::WindowMaximized);
    }
#endif
    w.set_settings(&settings);
    w.show();

    ret = a.exec();

    settings.beginGroup("mainwindow");
    settings.setValue("size", w.size());
    settings.setValue("pos", w.pos());
    settings.setValue("fullscreen", w.isFullScreen());
    settings.endGroup();
    return ret;
}
