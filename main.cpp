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
#include <QInputDialog>
#include <QMessageBox>
#include "common.h"
extern "C" {
#include <stdio.h>
#include <signal.h>
#include <openconnect.h>
#include <gnutls/pkcs11.h>
}

int pin_callback(void *userdata, int attempt, const char *token_url,
                 const char *token_label, unsigned flags, char *pin, size_t pin_max)
{
    MainWindow *w = (MainWindow*)userdata;
    QString text, outtext, type = "user";
    bool ok;

    if (flags & GNUTLS_PIN_SO)
        type = "security officer";

    outtext = "Please enter the " + type + " PIN for " + QLatin1String(token_label) + ".";
    if (flags & GNUTLS_PKCS11_PIN_FINAL_TRY)
        outtext += " This is the FINAL try!";

    if (flags & GNUTLS_PKCS11_PIN_COUNT_LOW)
        outtext += " Only few tries before token lock!";

    text = QInputDialog::getText(w, QLatin1String(token_url),
                                    outtext, QLineEdit::Normal,
                                    QString(), &ok);
    if (!ok)
        return -1;

    snprintf(pin, pin_max, "%s", text.toAscii().data());
    return 0;
}

int main(int argc, char *argv[])
{
    int ret;
    QApplication a(argc, argv);
    QVariant v;
    MainWindow w;
    QCoreApplication::setOrganizationDomain("redhat.com");
    QMessageBox msgBox;
    QSettings settings("Red Hat", "Qconnect");

    gnutls_global_init();
#ifndef _WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
    openconnect_init_ssl();

#ifdef ENABLE_PKCS11
    gnutls_pkcs11_set_pin_function(pin_callback, &w);
#endif

#if !defined(DEVEL)
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

#if !defined(_WIN32) && !defined(DEVEL)
    msgBox.setText("This program requires root privileges to fully function.");
    msgBox.setInformativeText("VPN connection establishment would fail.");
    ret = msgBox.exec();
#endif

    ret = a.exec();

    settings.beginGroup("mainwindow");
    settings.setValue("size", w.size());
    settings.setValue("pos", w.pos());
    settings.setValue("fullscreen", w.isFullScreen());
    settings.endGroup();
    return ret;
}
