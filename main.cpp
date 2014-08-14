/*
 * Copyright (C) 2014 Red Hat
 *
 * This file is part of qconnect.
 *
 * Qconnect is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
