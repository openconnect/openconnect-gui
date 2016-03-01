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

#ifndef VPNINFO_H
#define VPNINFO_H

#include <QString>
#ifdef _WIN32
#include <winsock2.h>
#endif

class MainWindow;
class StoredServer;

class VpnInfo {
public:
    VpnInfo(QString name, StoredServer* ss, MainWindow* m);
    ~VpnInfo();

    void parse_url(const char* url);
    int connect();
    int dtls_connect();
    void mainloop();
    void get_info(QString& dns, QString& ip, QString& ip6);
    void get_cipher_info(QString& cstp, QString& dtls);
    SOCKET get_cmd_fd() const;
    void reset_vpn();
    bool get_minimize() const;

    QString last_err;
    MainWindow* m;
    StoredServer* ss;
    struct openconnect_info* vpninfo;
    unsigned int authgroup_set;
    unsigned int password_set;
    unsigned int form_attempt;
    unsigned int form_pass_attempt;

private:
    SOCKET cmd_fd;
};

#endif // VPNINFO_H
