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

#include "MyInputDialog.h"

MyInputDialog::MyInputDialog(QWidget* w, QString t1, QString t2, QStringList list)
    : w(w)
    , t1(t1)
    , t2(t2)
    , list(list)
    , have_list(true)
{
    mutex.lock();
    this->moveToThread(QApplication::instance()->thread());
}

MyInputDialog::MyInputDialog(QWidget* w, QString t1, QString t2, QLineEdit::EchoMode type)
    : w(w)
    , t1(t1)
    , t2(t2)
    , have_list(false)
    , type(type)
{
    mutex.lock();
    this->moveToThread(QApplication::instance()->thread());
}

MyInputDialog::~MyInputDialog()
{
    mutex.tryLock();
    mutex.unlock();
}

void MyInputDialog::show()
{
    QCoreApplication::postEvent(this, new QEvent(QEvent::User));
}

bool MyInputDialog::event(QEvent* ev)
{
    res = false;
    if (ev->type() == QEvent::User) {
        if (this->have_list) {
            text = QInputDialog::getItem(w, t1, t2, list, 0, false, &res);
        } else {
            text = QInputDialog::getText(w, t1, t2, type, QString(), &res);
        }

        mutex.unlock();
    }
    return res;
}

bool MyInputDialog::result(QString& text)
{
    mutex.lock();
    mutex.unlock();
    text = this->text;
    return res;
}
