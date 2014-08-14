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
    void disconnect();
    int dtls_connect();
    void mainloop();

    const char *last_err;
    MainWindow *m;
    StoredServer *ss;
private:
    int cmd_fd;
    struct openconnect_info *vpninfo;
};

#endif // VPNINFO_H
