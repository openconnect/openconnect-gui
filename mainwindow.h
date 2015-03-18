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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QSettings>
#include <QFutureWatcher>
#include <QMutex>
#include "common.h"
#include <QTimer>
#include <QMenu>
#include <QSystemTrayIcon>
#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#else
#include <winsock2.h>
#endif

extern "C" {
#include <openconnect.h>
} namespace Ui {
    class MainWindow;
} enum status_t {
    STATUS_DISCONNECTED,
    STATUS_CONNECTING,
    STATUS_CONNECTED
};

class MainWindow:public QMainWindow {
 Q_OBJECT public:
     explicit MainWindow(QWidget * parent = 0);
    void updateProgressBar(QString str);
    void updateProgressBar(QString str, bool show);
    void set_settings(QSettings * s);
    void updateStats(const struct oc_stats *stats, QString dtls);
    void reload_settings();
    void toggleWindow();
    void setVisible(bool visible);
    void createActions();

    ~MainWindow();
    void disable_cmd_fd() {
        cmd_fd = INVALID_SOCKET;
    };

    void vpn_status_changed(int connected) {
        emit vpn_status_changed_sig(connected);
    };

    void vpn_status_changed(int connected, QString & dns, QString & ip,
                            QString & ip6, QString & cstp_cipher,
                            QString & dtls_cipher) {
        this->dns = dns;
        this->ip = ip;
        this->ip6 = ip6;
        this->dtls_cipher = dtls_cipher;
        this->cstp_cipher = cstp_cipher;
        emit vpn_status_changed_sig(connected);
    };

    QStringList *get_log(void) {
        return &this->log;
    }
    private slots:void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void statsChanged(QString, QString, QString);
    void writeProgressBar(QString str);
    void changeStatus(int);

    void blink_ui(void);
    void clear_logdialog(void);
    void clear_log(void);

    void request_update_stats();

    void on_disconnectBtn_clicked();

    void on_connectBtn_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_toolButton_3_clicked();
    void closeEvent(QCloseEvent * bar);

 signals:
    void log_changed(QString val);
    void stats_changed_sig(QString, QString, QString);
    void vpn_status_changed_sig(int);
    void timeout(void);

 private:
    void createTrayIcon();
    bool shown;
    /* we keep the fd instead of a pointer to vpninfo to avoid
     * any multithread issues */
    SOCKET cmd_fd;
    bool minimize_on_connect;
    Ui::MainWindow * ui;
    QSettings *settings;
    QMutex progress_mutex;
    QStringList log;
    QTimer *timer;
    QTimer *blink_timer;
    QFutureWatcher < void >futureWatcher;       // watches the vpninfo

    QString dns, ip, ip6;
    QString cstp_cipher;
    QString dtls_cipher;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;
};

#endif                          // MAINWINDOW_H
