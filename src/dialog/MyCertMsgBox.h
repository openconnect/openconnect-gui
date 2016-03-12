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

#pragma once

#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QMutex>

/* These input dialogs work from a different to main thread */
class MyCertMsgBox : public QObject {

public:
    MyCertMsgBox(QWidget* w, QString t1, QString t2, QString oktxt, QString details);
    ~MyCertMsgBox();

    void show();
    virtual bool event(QEvent* ev);
    bool result();

private:
    bool res;
    QMutex mutex;
    QWidget* w;
    QString t1;
    QString t2;
    QString oktxt;
    QString details;
};

