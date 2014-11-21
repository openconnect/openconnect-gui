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

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <QTranslator>

#define VERSION "0.8"
#define APP_NAME "openconnect-gui"
#define APP_STRING APP_NAME" "VERSION

#define toAscii toLatin1

/* undef on normal builds */
#undef DEVEL

#define TMP_CERT_PREFIX "tmp-certXXXXXX"
#define TMP_KEY_PREFIX "tmp-keyXXXXXX"

#define OFF_ICON QPixmap(QString::fromLatin1(":/new/resource/traffic_light_red.png"))
#define ON_ICON QPixmap(QString::fromLatin1(":/new/resource/traffic_light_green.png"))
#define CONNECTING_ICON QPixmap(QString::fromLatin1(":/new/resource/traffic_light_yellow.png"))
#define CONNECTING_ICON2 QPixmap(QString::fromLatin1(":/new/resource/traffic_light_off.png"))

#define UPDATE_TIMER 10000

#ifdef _WIN32
# define DEFAULT_VPNC_SCRIPT "vpnc-script-win.js"
# define net_errno WSAGetLastError()
# define ms_sleep Sleep
#else
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
# define ms_sleep(x) usleep(1000*x)
# define DEFAULT_VPNC_SCRIPT "/etc/vpnc/vpnc-script"
# define INVALID_SOCKET -1
# define SOCKET int
# define closesocket close
# define net_errno errno
#endif

#define USE_SYSTEM_KEYS

#include <QString>

inline bool is_url(QString &str)
{
    if (str.startsWith("system:") ||
        str.startsWith("pkcs11:") ||
        str.startsWith("system:")) {
            return true;
    }
    return false;
}

#endif // COMMON_H
