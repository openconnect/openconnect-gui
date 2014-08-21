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

#ifndef VPNINFO_H
#define VPNINFO_H

#include <mainwindow.h>
#include <storage.h>

extern "C" {
#include <openconnect.h>
}

class VpnInfo
{
public:
    explicit VpnInfo(const char *name, class StoredServer *ss, class MainWindow *m);
    ~VpnInfo();
    void parse_url(const char *url);
    int connect();
    int dtls_connect();
    void mainloop();
    void get_info(QString &dns, QString &ip, QString &ip6);
    SOCKET get_cmd_fd() {
        return cmd_fd;
    };

    const char *last_err;
    MainWindow *m;
    StoredServer *ss;
    struct openconnect_info *vpninfo;
private:
    SOCKET cmd_fd;
};

#endif // VPNINFO_H
