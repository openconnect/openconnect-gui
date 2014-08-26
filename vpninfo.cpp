/*
 * Copyright (C) 2014 Red Hat
 *
 * This file is part of openconnect-gui.
 *
 * openconnect-gui is free software: you can redistribute it and/or modify
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

#include <vpninfo.h>

extern "C" {
#include <stdarg.h>
#include <stdio.h>
}
#include "gtdb.h"
#include <QMessageBox>
#include <QInputDialog>
#include <dialogs.h>
#include <QApplication>

static void
stats_vfn (void *privdata, const struct oc_stats *stats)
{
    VpnInfo *vpn = static_cast<VpnInfo*>(privdata);

    vpn->m->updateStats(stats);
}

static
void progress_vfn(void *privdata, int level, const char *fmt, ...)
{
    VpnInfo *vpn = static_cast<VpnInfo*>(privdata);
    char buf[512];
    size_t len;
    va_list args;

    /* don't spam */
    if (level == PRG_TRACE)
    	return;

    buf[0] = 0;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    len = strlen(buf);
    if (buf[len-1] == '\n')
        buf[len-1] = 0;
    vpn->m->updateProgressBar(buf);
}

static
int process_auth_form(void *privdata, struct oc_auth_form *form)
{
    VpnInfo *vpn = static_cast<VpnInfo*>(privdata);
    bool ok;
    QString text;
    struct oc_form_opt *opt;
    QStringList gitems;
    int i;

    if (form->banner)
        vpn->m->updateProgressBar(form->banner);

    if (form->message)
        vpn->m->updateProgressBar(form->message);

    if (form->error) {
        vpn->m->updateProgressBar(form->error);
        return -1;
    }

    if (form->authgroup_opt) {
        struct oc_form_opt_select *select_opt = form->authgroup_opt;
        MyInputDialog dialog(vpn->m, QLatin1String(select_opt->form.name), QLatin1String(select_opt->form.label), gitems);

        for (i = 0; i < select_opt->nr_choices; i++) {
            gitems << select_opt->choices[i]->label;
        }

        /* if the configured exists */
        if (gitems.contains(vpn->ss->get_groupname())) {
            select_opt->form.value = openconnect_strdup(vpn->ss->get_groupname().toAscii().data());
            return OC_FORM_RESULT_NEWGROUP;
        }
        dialog.moveToThread( QApplication::instance()->thread());

        QCoreApplication::postEvent(&dialog, new QEvent( QEvent::User));
	ok = dialog.result(text);

        if (!ok) goto fail;

        vpn->ss->set_groupname(text);
        select_opt->form.value = openconnect_strdup(text.toAscii().data());
        return OC_FORM_RESULT_NEWGROUP;
    }

    for (opt = form->opts; opt; opt = opt->next) {
        text.clear();
        if (opt->flags & OC_FORM_OPT_IGNORE)
            continue;

        if (opt->type == OC_FORM_OPT_SELECT) {
            QStringList items;
            struct oc_form_opt_select *select_opt = reinterpret_cast<oc_form_opt_select*>(opt);
            MyInputDialog dialog(vpn->m, QLatin1String(opt->name), QLatin1String(opt->label), items);

            if (select_opt == form->authgroup_opt)
                continue;

            for (i = 0; i < select_opt->nr_choices; i++) {
                items << select_opt->choices[i]->label;
            }

            dialog.moveToThread( QApplication::instance()->thread());

            QCoreApplication::postEvent(&dialog, new QEvent( QEvent::User));
	    ok = dialog.result(text);

            if (!ok) goto fail;

            opt->value = openconnect_strdup(text.toAscii().data());

        } else if (opt->type == OC_FORM_OPT_TEXT) {
            if (vpn->ss->get_username().isEmpty() == false && strcmp(opt->name, "username") == 0) {
                opt->value = openconnect_strdup(vpn->ss->get_username().toAscii().data());
                return OC_FORM_RESULT_OK;
            }

            text.clear();
            do {
                MyInputDialog dialog(vpn->m, QLatin1String(opt->name), QLatin1String(opt->label), QLineEdit::Normal);
                dialog.moveToThread( QApplication::instance()->thread());

                QCoreApplication::postEvent(&dialog, new QEvent( QEvent::User));
	        ok = dialog.result(text);

                if (!ok) goto fail;
            } while(text.isEmpty());

            if (strcmp(opt->name, "username") == 0) {
                vpn->ss->set_username(text);
            }

            opt->value = openconnect_strdup(text.toAscii().data());

        } else if (opt->type == OC_FORM_OPT_PASSWORD) {
            if (vpn->ss->get_password().isEmpty() == false && strcmp(opt->name, "password") == 0) {
                opt->value = openconnect_strdup(vpn->ss->get_password().toAscii().data());
                return OC_FORM_RESULT_OK;
            }

            text.clear();
            do {
                MyInputDialog dialog(vpn->m, QLatin1String(opt->name), QLatin1String(opt->label), QLineEdit::Password);
                dialog.moveToThread( QApplication::instance()->thread());

                QCoreApplication::postEvent(&dialog, new QEvent( QEvent::User));
	        ok = dialog.result(text);

                if (!ok) goto fail;
            } while(text.isEmpty());

            if (strcmp(opt->name, "password") == 0) {
                vpn->ss->set_password(text);
            }
            opt->value = openconnect_strdup(text.toAscii().data());
        }
    }

    return OC_FORM_RESULT_OK;
 fail:
    return OC_FORM_RESULT_CANCELLED;
}

static
int validate_peer_cert(void *privdata, OPENCONNECT_X509 *cert, const char *reason)
{
    VpnInfo *vpn = static_cast<VpnInfo*>(privdata);
    unsigned char *der;
    int der_size, ret;
    gnutls_datum_t raw;
    char sha1_hash[41];
    QString str;
    gtdb tdb(vpn->ss);
    bool save = false;
    bool ok;

    der_size = openconnect_get_cert_DER(vpn->vpninfo,
                                        cert, &der);
    if (der_size <= 0) {
        vpn->m->updateProgressBar(QObject::tr("Peer's certificate has invalid size!"));
        return -1;
    }

    ret = openconnect_get_cert_sha1(vpn->vpninfo, cert, sha1_hash);
    if (ret != 0) {
        vpn->m->updateProgressBar(QObject::tr("Error getting peer's certificate hash"));
        return -1;
    }

    raw.data = der;
    raw.size = der_size;

    ret = gnutls_verify_stored_pubkey(reinterpret_cast<const char*>(&tdb), tdb.tdb, "", "", GNUTLS_CRT_X509, &raw, 0);

    if (ret == GNUTLS_E_NO_CERTIFICATE_FOUND) {
        vpn->m->updateProgressBar(QObject::tr("peer is unknown"));
        str = QObject::tr("Host: ") + vpn->ss->get_servername() + QObject::tr("\nSHA1: ") + sha1_hash;

        MyMsgBox msgBox(vpn->m, QObject::tr("You are connecting for the first time to this peer. Is the information provided below accurate?"),
        			str, QObject::tr("The information is accurate"));

        msgBox.moveToThread( QApplication::instance()->thread());
        QCoreApplication::postEvent(&msgBox, new QEvent( QEvent::User));
        ok = msgBox.result();

        if (ok == false)
            return -1;

        save = true;
    } else if (ret == GNUTLS_E_CERTIFICATE_KEY_MISMATCH) {
        vpn->m->updateProgressBar(QObject::tr("peer's key has changed!"));
        str = QObject::tr("Host: ")+ vpn->ss->get_servername() + QObject::tr("\nSHA1: ") + sha1_hash;

        MyMsgBox msgBox(vpn->m, QObject::tr("This peer is known and associated with a different key. It may be that the server has multiple keys or you are (or were in the past) under attack. Do you want to proceed?"),
        			str, QObject::tr("The key was changed by the administrator"));

        msgBox.moveToThread( QApplication::instance()->thread());
        QCoreApplication::postEvent(&msgBox, new QEvent( QEvent::User));
        ok = msgBox.result();

        if (ok == false)
            return -1;

        save = true;
    } else if (ret < 0) {
        str = QObject::tr("Could not verify certificate: ");
        str += gnutls_strerror(ret);
        vpn->m->updateProgressBar(str);
        return -1;
    }

    if (save != false) {
        vpn->m->updateProgressBar(QObject::tr("saving peer's public key"));
        ret = gnutls_store_pubkey(reinterpret_cast<const char*>(&tdb), tdb.tdb,
                                  "", "", GNUTLS_CRT_X509, &raw, 0, 0);
        if (ret < 0) {
            str = QObject::tr("Could not store certificate: ");
            str += gnutls_strerror(ret);
            vpn->m->updateProgressBar(str);
        }
    }
    return 0;
}

static inline int set_sock_block(int fd)
{
#ifdef _WIN32
	unsigned long mode = 0;
	return ioctlsocket(fd, FIONBIO, &mode);
#else
	return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
#endif
}

VpnInfo::VpnInfo(QString name, class StoredServer *ss, class MainWindow *m)
{
    this->vpninfo = openconnect_vpninfo_new(name.toAscii().data(), validate_peer_cert, NULL,
                                            process_auth_form, progress_vfn, this);
    if (this->vpninfo == NULL) {
        throw;
    }

    this->cmd_fd = openconnect_setup_cmd_pipe(vpninfo);
    if (this->cmd_fd == INVALID_SOCKET) {
        m->updateProgressBar(QObject::tr("invalid socket"));
        throw;
    }
    set_sock_block(this->cmd_fd);

    m->updateProgressBar(QObject::tr("socket ") + QString::number(this->cmd_fd));
    this->last_err = "";
    this->ss = ss;
    this->m = m;
    openconnect_set_stats_handler(this->vpninfo, stats_vfn);

}

VpnInfo::~VpnInfo()
{
    if (vpninfo)
        openconnect_vpninfo_free(vpninfo);

    if (this->ss)
        delete this->ss;
}

void VpnInfo::parse_url(const char *url)
{
    openconnect_parse_url(this->vpninfo, const_cast<char*>(url));
}

int VpnInfo::connect()
{
    int ret;
    QString cert_file, key_file;

    cert_file = ss->get_cert_file();
    key_file = ss->get_key_file();

    if (key_file.isEmpty() == true)
        key_file = cert_file;

    if (cert_file.isEmpty() != true) {
        openconnect_set_client_cert(vpninfo, openconnect_strdup(cert_file.toAscii().data()),
                                    openconnect_strdup(key_file.toAscii().data()));
    }

    openconnect_set_reported_os(vpninfo, "Windows");

    ret = openconnect_obtain_cookie(vpninfo);
    if (ret != 0) {
        this->last_err = QObject::tr("Authentication error; cannot obtain cookie");
        return ret;
    }

    ret = openconnect_make_cstp_connection(vpninfo);
    if (ret != 0) {
        this->last_err = QObject::tr("Error establishing the CSTP channel");
        return ret;
    }

    ret = openconnect_setup_tun_device(vpninfo, openconnect_strdup(DEFAULT_VPNC_SCRIPT), NULL);
    if (ret != 0) {
        this->last_err = QObject::tr("Error setting up the TUN device");
        return ret;
    }

    return 0;
}

int VpnInfo::dtls_connect()
{
    int ret;

    ret = openconnect_setup_dtls(vpninfo, 60);
    if (ret != 0) {
        this->last_err = QObject::tr("Error setting up DTLS");
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
            this->last_err = QObject::tr("Disconnected");
            //this->ss->save();
            break;
        }
    }

}

void VpnInfo::get_info(QString &dns, QString &ip, QString &ip6)
{
    const struct oc_ip_info *info;
    int ret;
    ret = openconnect_get_ip_info(this->vpninfo, &info, NULL, NULL);
    if (ret == 0) {
        if (info->addr) {
            ip = info->addr;
            if (info->netmask) {
                ip += "/";
                ip += info->netmask;
            }
        }
        if (info->addr6) {
            ip6 = info->addr6;
            if (info->netmask6) {
                ip6 += "/";
                ip6 += info->netmask6;
            }
        }

        dns = info->dns[0];
        if (info->dns[1]) {
            dns += ", ";
            dns += info->dns[1];
        }
        if (info->dns[2]) {
            dns += " ";
            dns += info->dns[2];
        }
    }
    return;
}
