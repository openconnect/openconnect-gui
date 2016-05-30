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

#include "vpninfo.h"
#include "config.h"
#include "dialog/MyCertMsgBox.h"
#include "dialog/MyInputDialog.h"
#include "dialog/mainwindow.h"
#include "gtdb.h"
#include "storage.h"

#include <QDir>

#include <cstdarg>
#include <cstdio>

static void stats_vfn(void* privdata, const struct oc_stats* stats)
{
    VpnInfo* vpn = static_cast<VpnInfo*>(privdata);
    const char* cipher;
    QString dtls;

    cipher = openconnect_get_dtls_cipher(vpn->vpninfo);
    if (cipher != nullptr) {
        dtls = QLatin1String(cipher);
    }

    vpn->m->updateStats(stats, dtls);
}

static void progress_vfn(void* privdata, int level, const char* fmt, ...)
{
    VpnInfo* vpn = static_cast<VpnInfo*>(privdata);
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
    if (buf[len - 1] == '\n')
        buf[len - 1] = 0;
    vpn->m->updateProgressBar(buf);
}

static int process_auth_form(void* privdata, struct oc_auth_form* form)
{
    VpnInfo* vpn = static_cast<VpnInfo*>(privdata);
    bool ok;
    QString text;
    struct oc_form_opt* opt;
    QStringList gitems;
    QStringList ditems;
    int i, idx;

    if (form->banner)
        vpn->m->updateProgressBar(QLatin1String(form->banner));

    if (form->message)
        vpn->m->updateProgressBar(QLatin1String(form->message));

    if (form->error) {
        vpn->m->updateProgressBar(QLatin1String(form->error));
        return -1;
    }

    if (form->authgroup_opt) {
        struct oc_form_opt_select* select_opt = form->authgroup_opt;

        for (i = 0; i < select_opt->nr_choices; i++) {
            ditems << select_opt->choices[i]->label;
            gitems << select_opt->choices[i]->name;
        }

        /* if the configured exists */
        if (gitems.contains(vpn->ss->get_groupname())) {
            openconnect_set_option_value(&select_opt->form,
                vpn->ss->get_groupname().toLatin1().data());
        } else {
            {
                MyInputDialog dialog(vpn->m,
                    QLatin1String(select_opt->form.name),
                    QLatin1String(select_opt->form.label),
                    ditems);
                dialog.show();
                ok = dialog.result(text);
            }

            if (!ok)
                goto fail;

            idx = ditems.indexOf(text);
            if (idx == -1)
                goto fail;

            openconnect_set_option_value(&select_opt->form,
                select_opt->choices[idx]->name);
            text = QLatin1String(select_opt->choices[idx]->name);

            vpn->m->updateProgressBar(QLatin1String("Saving group: ") + text);
            vpn->ss->set_groupname(text);
        }

        if (vpn->authgroup_set == 0) {
            vpn->authgroup_set = 1;
            return OC_FORM_RESULT_NEWGROUP;
        }
    }

    for (opt = form->opts; opt; opt = opt->next) {
        text.clear();
        if (opt->flags & OC_FORM_OPT_IGNORE)
            continue;

        if (opt->type == OC_FORM_OPT_SELECT) {
            QStringList items;
            struct oc_form_opt_select* select_opt = reinterpret_cast<oc_form_opt_select*>(opt);

            vpn->m->updateProgressBar(QLatin1String("Select form: ") + QLatin1String(opt->name));

            if (select_opt == form->authgroup_opt) {
                continue;
            }

            for (i = 0; i < select_opt->nr_choices; i++) {
                items << select_opt->choices[i]->label;
            }

            {
                MyInputDialog dialog(vpn->m, QLatin1String(opt->name),
                    QLatin1String(opt->label), items);
                dialog.show();
                ok = dialog.result(text);
            }

            if (!ok)
                goto fail;

            idx = ditems.indexOf(text);
            if (idx == -1)
                goto fail;

            openconnect_set_option_value(opt, select_opt->choices[idx]->name);
        } else if (opt->type == OC_FORM_OPT_TEXT) {
            vpn->m->updateProgressBar(QLatin1String("Text form: ") + QLatin1String(opt->name));

            if (vpn->form_attempt == 0
                && vpn->ss->get_username().isEmpty() == false
                && strcasecmp(opt->name, "username") == 0) {
                openconnect_set_option_value(opt,
                    vpn->ss->get_username().toLatin1().data());
                continue;
            }

            do {
                MyInputDialog dialog(vpn->m, QLatin1String(opt->name),
                    QLatin1String(opt->label),
                    QLineEdit::Normal);
                dialog.show();
                ok = dialog.result(text);

                if (!ok)
                    goto fail;
            } while (text.isEmpty());

            if (strcasecmp(opt->name, "username") == 0) {
                vpn->ss->set_username(text);
            }

            openconnect_set_option_value(opt, text.toLatin1().data());
            vpn->form_attempt++;
        } else if (opt->type == OC_FORM_OPT_PASSWORD) {
            vpn->m->updateProgressBar(QLatin1String("Password form: ") + QLatin1String(opt->name));

            if (vpn->form_pass_attempt == 0
                && vpn->ss->get_password().isEmpty() == false
                && strcasecmp(opt->name, "password") == 0) {
                openconnect_set_option_value(opt,
                    vpn->ss->get_password().toLatin1().data());
                continue;
            }

            do {
                MyInputDialog dialog(vpn->m, QLatin1String(opt->name),
                    QLatin1String(opt->label),
                    QLineEdit::Password);
                dialog.show();
                ok = dialog.result(text);

                if (!ok)
                    goto fail;
            } while (text.isEmpty() == true);

            if (strcasecmp(opt->name, "password") == 0
                && (vpn->password_set == 0 || vpn->form_pass_attempt != 0)) {
                vpn->ss->set_password(text);
                vpn->password_set = 1;
            }
            openconnect_set_option_value(opt, text.toLatin1().data());
            vpn->form_pass_attempt++;
        } else {
            vpn->m->updateProgressBar(QLatin1String("unknown type ") + QString::number((int)opt->type));
        }
    }

    return OC_FORM_RESULT_OK;
fail:
    return OC_FORM_RESULT_CANCELLED;
}

static int validate_peer_cert(void* privdata, const char* reason)
{
    VpnInfo* vpn = static_cast<VpnInfo*>(privdata);
    unsigned char* der = nullptr;
    int der_size = openconnect_get_peer_cert_DER(vpn->vpninfo, &der);
    if (der_size <= 0) {
        vpn->m->updateProgressBar(QObject::tr("Peer's certificate has invalid size!"));
        return -1;
    }

    const char* hash = openconnect_get_peer_cert_hash(vpn->vpninfo);
    if (hash == nullptr) {
        vpn->m->updateProgressBar(QObject::tr("Error getting peer's certificate hash"));
        return -1;
    }

    gnutls_datum_t raw;
    raw.data = der;
    raw.size = der_size;

    gtdb tdb(vpn->ss);
    int ret = gnutls_verify_stored_pubkey(reinterpret_cast<const char*>(&tdb),
        tdb.tdb, "", "", GNUTLS_CRT_X509, &raw, 0);

    char* details = openconnect_get_peer_cert_details(vpn->vpninfo);
    QString dstr;
    if (details != nullptr) {
        dstr = QString::fromUtf8(details);
        free(details);
    }

    bool save = false;
    if (ret == GNUTLS_E_NO_CERTIFICATE_FOUND) {
        vpn->m->updateProgressBar(QObject::tr("peer is unknown"));

        QString str = QObject::tr("Host: ") + vpn->ss->get_servername() + QObject::tr("\n") + hash;

        MyCertMsgBox msgBox(vpn->m,
            QObject::tr("You are connecting for the first time to this peer."
                        "Is the information provided below accurate?"),
            str,
            QObject::tr("Accurate information"),
            dstr);
        msgBox.show();
        if (msgBox.result() == false) {
            return -1;
        }

        save = true;
    } else if (ret == GNUTLS_E_CERTIFICATE_KEY_MISMATCH) {
        vpn->m->updateProgressBar(QObject::tr("peer's key has changed!"));

        QString str = QObject::tr("Host: ") + vpn->ss->get_servername() + QObject::tr("\n") + hash;

        MyCertMsgBox msgBox(vpn->m,
            QObject::tr("This peer is known and associated with a different key."
                        "It may be that the server has multiple keys "
                        "or you are (or were in the past) under attack. "
                        "Do you want to proceed?"),
            str,
            QObject::tr("The key was changed by the administrator"),
            dstr);
        msgBox.show();
        if (msgBox.result() == false) {
            return -1;
        }

        save = true;
    } else if (ret < 0) {
        QString str = QObject::tr("Could not verify certificate: ");
        str += gnutls_strerror(ret);
        vpn->m->updateProgressBar(str);
        return -1;
    }

    if (save != false) {
        vpn->m->updateProgressBar(QObject::tr("saving peer's public key"));
        ret = gnutls_store_pubkey(reinterpret_cast<const char*>(&tdb), tdb.tdb,
            "", "", GNUTLS_CRT_X509, &raw, 0, 0);
        if (ret < 0) {
            QString str = QObject::tr("Could not store certificate: ");
            str += gnutls_strerror(ret);
            vpn->m->updateProgressBar(str);
        } else {
            vpn->ss->save();
        }
    }
    return 0;
}

static int lock_token_vfn(void* privdata)
{
    VpnInfo* vpn = static_cast<VpnInfo*>(privdata);

    openconnect_set_token_mode(vpn->vpninfo,
        (oc_token_mode_t)vpn->ss->get_token_type(),
        vpn->ss->get_token_str().toLatin1().data());

    return 0;
}

static int unlock_token_vfn(void* privdata, const char* newtok)
{
    VpnInfo* vpn = static_cast<VpnInfo*>(privdata);

    vpn->ss->set_token_str(newtok);
    vpn->ss->save();
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

VpnInfo::VpnInfo(QString name, StoredServer* ss, MainWindow* m)
{
    this->vpninfo = openconnect_vpninfo_new(name.toLatin1().data(), validate_peer_cert, nullptr,
        process_auth_form, progress_vfn, this);
    if (this->vpninfo == nullptr) {
        throw;
    }

    this->cmd_fd = openconnect_setup_cmd_pipe(vpninfo);
    if (this->cmd_fd == INVALID_SOCKET) {
        m->updateProgressBar(QObject::tr("invalid socket"));
        throw;
    }
    set_sock_block(this->cmd_fd);

    this->last_err = "";
    this->ss = ss;
    this->m = m;
    authgroup_set = 0;
    password_set = 0;
    form_attempt = 0;
    form_pass_attempt = 0;
    openconnect_set_stats_handler(this->vpninfo, stats_vfn);
    if (ss->get_token_str().isEmpty() == false) {
        openconnect_set_token_callbacks(this->vpninfo, this, lock_token_vfn, unlock_token_vfn);
        openconnect_set_token_mode(this->vpninfo,
            (oc_token_mode_t)ss->get_token_type(),
            ss->get_token_str().toLatin1().data());
    }
}

VpnInfo::~VpnInfo()
{
    if (vpninfo != nullptr) {
        openconnect_vpninfo_free(vpninfo);
    }

    if (ss != nullptr) {
        delete ss;
    }
}

void VpnInfo::parse_url(const char* url)
{
    openconnect_parse_url(this->vpninfo, const_cast<char*>(url));
}

int VpnInfo::connect()
{
    int ret;
    QString cert_file, key_file;
    QString ca_file;

    cert_file = ss->get_cert_file();
    ca_file = ss->get_ca_cert_file();
    key_file = ss->get_key_file();

    if (key_file.isEmpty() == true)
        key_file = cert_file;

    if (cert_file.isEmpty() != true) {
        openconnect_set_client_cert(vpninfo, cert_file.toLatin1().data(),
            key_file.toLatin1().data());
    }

    if (ca_file.isEmpty() != true) {
        openconnect_set_system_trust(vpninfo, 0);
        openconnect_set_cafile(vpninfo, ca_file.toLatin1().data());
    }

    openconnect_set_reported_os(vpninfo, "win");

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

    QByteArray vpncScriptFullPath;
    vpncScriptFullPath.append(QCoreApplication::applicationDirPath());
    vpncScriptFullPath.append(QDir::separator());
    vpncScriptFullPath.append(DEFAULT_VPNC_SCRIPT);
    ret = openconnect_setup_tun_device(vpninfo, vpncScriptFullPath.constData(), NULL);
    if (ret != 0) {
        this->last_err = QObject::tr("Error setting up the TUN device");
        return ret;
    }

    logVpncScriptOutput();

    return 0;
}

int VpnInfo::dtls_connect()
{
    if (this->ss->get_disable_udp() != true) {
        int ret = openconnect_setup_dtls(vpninfo, 60);
        if (ret != 0) {
            this->last_err = QObject::tr("Error setting up DTLS");
            return ret;
        }
    }

    return 0;
}

void VpnInfo::mainloop()
{
    while (true) {
        int ret = openconnect_mainloop(vpninfo, 15, RECONNECT_INTERVAL_MIN);
        if (ret != 0) {
            this->last_err = QObject::tr("Disconnected");
            logVpncScriptOutput();
            break;
        }
    }
}

void VpnInfo::get_info(QString& dns, QString& ip, QString& ip6)
{
    const struct oc_ip_info* info;
    int ret = openconnect_get_ip_info(this->vpninfo, &info, nullptr, nullptr);
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

void VpnInfo::get_cipher_info(QString& cstp, QString& dtls)
{
    const char* cipher = openconnect_get_cstp_cipher(this->vpninfo);
    if (cipher != nullptr) {
        cstp = QLatin1String(cipher);
    }
    cipher = openconnect_get_dtls_cipher(this->vpninfo);
    if (cipher != nullptr) {
        dtls = QLatin1String(cipher);
    }
}

SOCKET VpnInfo::get_cmd_fd() const
{
    return cmd_fd;
}

void VpnInfo::reset_vpn()
{
    openconnect_reset_ssl(vpninfo);
    form_pass_attempt = 0;
    password_set = 0;
    authgroup_set = 0;
    form_attempt = 0;
}

bool VpnInfo::get_minimize() const
{
    return ss->get_minimize();
}

void VpnInfo::logVpncScriptOutput()
{
    /* now read %temp%\\vpnc.log and post it to our log */
    QString tfile = QDir::tempPath() + QDir::separator() + QLatin1String("vpnc.log");
    QFile file(tfile);
    if (file.open(QIODevice::ReadOnly) == true) {
        QTextStream in(&file);

        while (!in.atEnd()) {
            QString line = in.readLine();
            this->m->updateProgressBar(line, false);
        }
        file.close();
        QFile::remove(tfile);
    } else {
        this->m->updateProgressBar(QLatin1String("Could not open ") + tfile + ": " + QString::number((int)file.error()));
    }

}
