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

/* undef on normal builds */
#undef PROJ_GNUTLS_DEBUG

#define TMP_CERT_PREFIX "tmp-certXXXXXX"
#define TMP_KEY_PREFIX "tmp-keyXXXXXX"

#define OFF_ICON QPixmap(QString::fromLatin1(":/new/resource/images/traffic_light_red.png"))
#define ON_ICON QPixmap(QString::fromLatin1(":/new/resource/images/traffic_light_green.png"))
#define CONNECTING_ICON QPixmap(QString::fromLatin1(":/new/resource/images/traffic_light_yellow.png"))
#define CONNECTING_ICON2 QPixmap(QString::fromLatin1(":/new/resource/images/traffic_light_off.png"))

#define TRAY_OFF_ICON QPixmap(QString::fromLatin1(":/new/resource/images/network-disconnected.png"))
#define TRAY_ON_ICON QPixmap(QString::fromLatin1(":/new/resource/images/network-connected.png"))

#define UPDATE_TIMER 10000

#ifdef _WIN32
#define net_errno WSAGetLastError()
#define ms_sleep Sleep
#else
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#define ms_sleep(x) usleep(1000 * x)
#define INVALID_SOCKET -1
#define SOCKET int
#define closesocket close
#define net_errno errno
#endif

extern "C" {
#include <gnutls/gnutls.h>
}

#if !defined(__MACH__) && GNUTLS_VERSION_NUMBER >= 0x030400
#define USE_SYSTEM_KEYS
#endif

#include <QString>

inline bool is_url(QString& str)
{
    if (str.startsWith("system:") || str.startsWith("pkcs11:") || str.startsWith("system:")) {
        return true;
    }
    return false;
}

#endif // COMMON_H
