#ifndef VPNINFO_H
#define VPNINFO_H

#include <mainwindow.h>

extern "C" {
#include <openconnect.h>
}

class VpnInfo
{
public:
    explicit VpnInfo(const char *name, class MainWindow *m);
    ~VpnInfo();
    void parse_url(const char *url);
    int connect();
    int dtls_connect();
    void mainloop();

    const char *last_err;
private:
    struct openconnect_info *vpninfo;
    class MainWindow *m;
    int cmd_fd;
};

#endif // VPNINFO_H
