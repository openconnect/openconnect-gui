/*
 * Copyright (C) 2014 Red Hat
 *
 * This file is part of openconnect-gui.
 *
 * openconnect-gui is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
#include <QMessageBox>
#include <dialogs.h>
#include "common.h"
extern "C" {
#include <stdio.h>
#include <signal.h>
#include <openconnect.h>
#include <gnutls/pkcs11.h>
} static QStringList *log = NULL;

int pin_callback(void *userdata, int attempt, const char *token_url,
                 const char *token_label, unsigned flags, char *pin,
                 size_t pin_max)
{
    MainWindow *w = (MainWindow *) userdata;
    QString text, outtext, type = "user";
    bool ok;

    if (flags & GNUTLS_PIN_SO)
        type = QObject::tr("security officer");

    outtext =
        QObject::tr("Please enter the ") + type + QObject::tr(" PIN for ") +
        QLatin1String(token_label) + ".";
    if (flags & GNUTLS_PKCS11_PIN_FINAL_TRY)
        outtext += QObject::tr(" This is the FINAL try!");

    if (flags & GNUTLS_PKCS11_PIN_COUNT_LOW)
        outtext += QObject::tr(" Only few tries before token lock!");

    MyInputDialog dialog(w, QLatin1String(token_url), outtext,
                         QLineEdit::Password);
    dialog.show();
    ok = dialog.result(text);

    if (!ok)
        return -1;

    snprintf(pin, pin_max, "%s", text.toAscii().data());
    return 0;
}

static void log_func(int level, const char *str)
{
    if (log != NULL) {
        QString s = QLatin1String(str);
        log->append(s.trimmed());
    }
}

int main(int argc, char *argv[])
{
    int ret;
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    QVariant v;
    MainWindow w;
    QCoreApplication::setOrganizationDomain("redhat.com");
    QMessageBox msgBox;
    QSettings settings("Red Hat", "openconnect-gui");

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
    if (getuid() != 0) {
        msgBox.setText(QObject::tr
                       ("This program requires root privileges to fully function."));
        msgBox.setInformativeText(QObject::tr
                                  ("VPN connection establishment would fail."));
        ret = msgBox.exec();
    }
#endif

#ifdef DEVEL
    gnutls_global_set_log_function(log_func);
    gnutls_global_set_log_level(3);
    log = w.get_log();
    log_func(1, "started logging");
#endif

    ret = a.exec();

    settings.beginGroup("mainwindow");
    settings.setValue("size", w.size());
    settings.setValue("pos", w.pos());
    settings.setValue("fullscreen", w.isFullScreen());
    settings.endGroup();

    return ret;
}
