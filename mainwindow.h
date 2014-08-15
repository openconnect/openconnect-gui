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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QSettings>
#include <QMutex>
#include "common.h"

extern "C" {
#include <openconnect.h>
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void updateProgressBar(const char *str);
    void enableDisconnect(bool);
    void set_settings(QSettings *s);
    void reload_settings();
    ~MainWindow();

private slots:
    void on_disconnectBtn_clicked();

    void on_connectBtn_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

private:
    class VpnInfo *vpninfo;
    Ui::MainWindow *ui;
    QSettings *settings;
    QMutex progress_mutex;
};

#endif // MAINWINDOW_H
