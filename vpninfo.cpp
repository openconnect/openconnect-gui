#include <vpninfo.h>

extern "C" {
#include <stdarg.h>
#include <stdio.h>
}

#include <QMessageBox>
#include <QInputDialog>

static
void progress_vfn(void *privdata, int level, const char *fmt, ...)
{
    MainWindow *m = static_cast<MainWindow*>(privdata);
    char buf[512];
    size_t len;
    va_list args;

    buf[0] = 0;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    len = strlen(buf);
    if (buf[len-1] == '\n')
        buf[len-1] = 0;
    m->updateProgressBar(buf, -1);
}

static
int process_auth_form(void *privdata, struct oc_auth_form *form)
{
    MainWindow *m = static_cast<MainWindow*>(privdata);
    bool ok;
    QString text;
    struct oc_form_opt *opt;
    QStringList gitems;
    int i;

    if (form->banner)
        m->updateProgressBar(form->banner, -1);

    if (form->message)
        m->updateProgressBar(form->message, -1);

    if (form->error) {
        m->updateProgressBar(form->error, -1);
        return -1;
    }

    if (form->authgroup_opt) {
        struct oc_form_opt_select *select_opt = form->authgroup_opt;

        for (i = 0; i < select_opt->nr_choices; i++) {
            gitems << select_opt->choices[i]->label;
        }

        text = QInputDialog::getItem(m, QLatin1String(select_opt->form.name),
                                        QLatin1String(select_opt->form.label), gitems, 0,
                                        true, &ok);
        if (!ok) goto fail;
        select_opt->form.value = strdup(text.toAscii().data());
        return OC_FORM_RESULT_NEWGROUP;
    }

    for (opt = form->opts; opt; opt = opt->next) {
        if (opt->flags & OC_FORM_OPT_IGNORE)
            continue;

        if (opt->type == OC_FORM_OPT_SELECT) {
            QStringList items;
            struct oc_form_opt_select *select_opt = reinterpret_cast<oc_form_opt_select*>(opt);

            if (select_opt == form->authgroup_opt)
                continue;

            for (i = 0; i < select_opt->nr_choices; i++) {
                items << select_opt->choices[i]->label;
            }

            text = QInputDialog::getItem(m, QLatin1String(opt->name),
                                            QLatin1String(opt->label), items, 0,
                                            true, &ok);
            if (!ok) goto fail;
            opt->value = strdup(text.toAscii().data());

        } else if (opt->type == OC_FORM_OPT_TEXT) {
            do {
                text = QInputDialog::getText(m, QLatin1String(opt->name),
                                                QLatin1String(opt->label), QLineEdit::Normal,
                                                QString(), &ok);
                if (!ok) goto fail;
            } while(text.isEmpty());
            opt->value = strdup(text.toAscii().data());

        } else if (opt->type == OC_FORM_OPT_PASSWORD) {
            do {
                text = QInputDialog::getText(m, QLatin1String(opt->name),
                                                QLatin1String(opt->label), QLineEdit::Password,
                                                QString(), &ok);
                if (!ok) goto fail;
            } while(text.isEmpty());
                opt->value = strdup(text.toAscii().data());
        }
    }

    return OC_FORM_RESULT_OK;
 fail:
    return OC_FORM_RESULT_CANCELLED;
}

static
int validate_peer_cert(void *privdata, OPENCONNECT_X509 *cert, const char *reason)
{
    return 0;
}

VpnInfo::VpnInfo(const char *name, class MainWindow *m)
{
    char *p = const_cast<char*>(name);
    this->vpninfo = openconnect_vpninfo_new(p, validate_peer_cert, NULL,
                                            process_auth_form, progress_vfn, m);
    if (this->vpninfo == NULL) {
        throw;
    }

    this->cmd_fd = openconnect_setup_cmd_pipe(vpninfo);
    this->last_err = NULL;
    openconnect_set_reported_os(vpninfo, "Windows");
}

VpnInfo::~VpnInfo()
{
    if (this->cmd_fd != -1)
        close(this->cmd_fd);

    openconnect_vpninfo_free(vpninfo);
}

void VpnInfo::parse_url(const char *url)
{
    openconnect_parse_url(this->vpninfo, const_cast<char*>(url));
}

#define DEFAULT_VPNC_SCRIPT "/etc/vpnc/vpnc-script"

int VpnInfo::connect()
{
    int ret;

    ret = openconnect_obtain_cookie(vpninfo);
    if (ret != 0) {
        this->last_err = "Error obtaining cookie";
        return ret;
    }

    ret = openconnect_make_cstp_connection(vpninfo);
    if (ret != 0) {
        this->last_err = "Error establishing the CSTP channel";
        return ret;
    }

    ret = openconnect_setup_tun_device(vpninfo, strdup(DEFAULT_VPNC_SCRIPT), NULL);
    if (ret != 0) {
        this->last_err = "Error setting up the TUN device";
        return ret;
    }

    return 0;
}

int VpnInfo::dtls_connect()
{
    int ret;

    ret = openconnect_setup_dtls(vpninfo, 60);
    if (ret != 0) {
        this->last_err = "Error setting up DTLS";
        return ret;
    }

    return 0;
}

void VpnInfo::mainloop()
{
    int ret;

    while(1) {
        ret = openconnect_mainloop(vpninfo, 30, RECONNECT_INTERVAL_MIN);
        if (ret != 0) {
            this->last_err = "Disconnected";
            break;
        }
    }

}
