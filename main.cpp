#include "mainwindow.h"
#include <QApplication>
extern "C" {
#include <openconnect.h>
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    openconnect_init_ssl();

    w.show();

    return a.exec();
}
