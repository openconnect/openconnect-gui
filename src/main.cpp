/*
 * Copyright (C) 2014 Red Hat
 * Copyright (C) 2016 by Lubomír Carik <Lubomir.Carik@gmail.com>
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

#include "common.h"
#include "config.h"
#include "dialog/MyInputDialog.h"
#include "dialog/mainwindow.h"
#include "openconnect-gui.h"

#include "logger.h"
#include "FileLogger.h"

extern "C" {
#include <gnutls/pkcs11.h>
#include <openconnect.h>
}

#include <QApplication>
#if !defined(_WIN32) && !defined(PROJ_GNUTLS_DEBUG)
#include <QMessageBox>
#endif
#include <QSettings>
#include <QCommandLineParser>
#include <QtSingleApplication>

#ifdef __MACH__
#include <mach-o/dyld.h>
#include <Security/Security.h>
#endif

#include <csignal>
#include <cstdio>

static void log_callback(int level, const char* str)
{
    Logger::instance().addMessage(QString(str).trimmed(),
                                  Logger::MessageType::DEBUG,
                                  Logger::ComponentType::GNUTLS
                                  );
}

#ifdef __MACH__
bool relaunch_as_root()
{
    QMessageBox msgBox;
    char appPath[2048];
    uint32_t size = sizeof(appPath);
    AuthorizationRef authRef;
    OSStatus status;

    /* Get the path of the current program */
    if (_NSGetExecutablePath(appPath, &size) != 0) {
        msgBox.setText(QObject::tr
            ("Could not get program path to elevate privileges."));
        return false;
    }

    status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment,
        kAuthorizationFlagDefaults, &authRef);

    if (status != errAuthorizationSuccess) {
        msgBox.setText(QObject::tr
            ("Failed to create authorization reference."));
        return false;
    }
    status = AuthorizationExecuteWithPrivileges(authRef, appPath,
        kAuthorizationFlagDefaults, NULL, NULL);
    AuthorizationFree(authRef, kAuthorizationFlagDestroyRights);

    if (status == errAuthorizationSuccess) {
        /* We've successfully re-launched with root privs. */
        return true;
    }

    return false;
}
#endif

int pin_callback(void* userdata, int attempt, const char* token_url,
    const char* token_label, unsigned flags, char* pin,
    size_t pin_max)
{
    QString type = QObject::tr("user");
    if (flags & GNUTLS_PIN_SO) {
        type = QObject::tr("security officer");
    }

    QString outtext = QObject::tr("Please enter the ") + type + QObject::tr(" PIN for ") + QLatin1String(token_label) + ".";
    if (flags & GNUTLS_PKCS11_PIN_FINAL_TRY) {
        outtext += QObject::tr(" This is the FINAL try!");
    }
    if (flags & GNUTLS_PKCS11_PIN_COUNT_LOW) {
        outtext += QObject::tr(" Only few tries before token lock!");
    }

    MainWindow* w = (MainWindow*)userdata;
    MyInputDialog dialog(w, QLatin1String(token_url), outtext, QLineEdit::Password);
    dialog.show();

    QString text;
    bool ok = dialog.result(text);
    if (ok == false) {
        return -1;
    }

    snprintf(pin, pin_max, "%s", text.toLatin1().data());
    return 0;
}

int main(int argc, char* argv[])
{
#if !defined(Q_OS_MACOS)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    qRegisterMetaType<Logger::Message>();

#ifdef PROJ_INI_SETTINGS
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

#ifdef __MACH__
    /* Re-launching with root privs on OS X needs Qt to allow setsuid */
    QApplication::setSetuidAllowed(true);
#endif
    QCoreApplication::setApplicationName(appDescription);
    QCoreApplication::setApplicationVersion(appVersion);
    QCoreApplication::setOrganizationName(appOrganizationName);
    QCoreApplication::setOrganizationDomain(appOrganizationDomain);

    QtSingleApplication  app(argc, argv);
    if (app.isRunning()) {
        QSettings settings;
        if (settings.value(QLatin1Literal("Settings/singleInstanceMode"), true).toBool()) {
            app.sendMessage("Wake up!");
            return 0;
        }
    }
    app.setApplicationDisplayName(appDescriptionLong);
    app.setQuitOnLastWindowClosed(false);

    auto fileLog = std::make_unique<FileLogger>();
    Logger::instance().addMessage(QString("%1 (%2) logging started...").arg(app.applicationDisplayName()).arg(app.applicationVersion()));

#ifdef __MACH__
    if (geteuid() != 0) {
        if (relaunch_as_root()) {
            /* We have re-launched with root privs. Exit this process. */
            return 0;
        }

        QMessageBox msgBox;
        msgBox.setText(QObject::tr("This program requires root privileges to fully function."));
        msgBox.setInformativeText(QObject::tr("VPN connection establishment would fail."));
        msgBox.exec();
    }
#endif

    gnutls_global_init();
#ifndef _WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
    openconnect_init_ssl();

    QCommandLineParser parser;
    parser.setApplicationDescription(
                QObject::tr("OpenConnect is a VPN client, that utilizes TLS and DTLS "
                            "for secure session establishment, and is compatible "
                            "with the CISCO AnyConnect SSL VPN protocol."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption({
                         {"s", "server"},
                         QObject::tr("auto-connect to existing profile <name>"),
                         QObject::tr("name")

                     });

    parser.process(app);

    const QString profileName{parser.value(QLatin1String("server"))};
    MainWindow mainWindow(nullptr, profileName);
    app.setActivationWindow(&mainWindow);
#ifdef PROJ_PKCS11
    gnutls_pkcs11_set_pin_function(pin_callback, &mainWindow);
#endif
    gnutls_global_set_log_function(log_callback);
#ifdef PROJ_GNUTLS_DEBUG
    gnutls_global_set_log_level(3);
#endif

    mainWindow.show();
    QObject::connect(&app, &QtSingleApplication::messageReceived,
                     [&mainWindow](const QString &message) {
        Logger::instance().addMessage(message);
    }
    );
    return app.exec();
}
