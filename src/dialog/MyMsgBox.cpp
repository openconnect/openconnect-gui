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

#include "MyMsgBox.h"

MyMsgBox::MyMsgBox(QWidget* w, QString t1, QString t2, QString oktxt) :
    w(w),
    t1(t1),
    t2(t2),
    oktxt(oktxt)

{
    mutex.lock();
    this->moveToThread(QApplication::instance()->thread());
}

MyMsgBox::~MyMsgBox()
{
    mutex.tryLock();
    mutex.unlock();
}

void MyMsgBox::show()
{
    QCoreApplication::postEvent(this, new QEvent(QEvent::User));
}

bool MyMsgBox::event(QEvent* ev)
{
    res = false;
    if (ev->type() == QEvent::User) {
        QMessageBox* msgBox = new QMessageBox(w);
        msgBox->setText(t1);
        msgBox->setInformativeText(t2);
        msgBox->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox->setDefaultButton(QMessageBox::Cancel);
        msgBox->setButtonText(QMessageBox::Ok, oktxt);

        if (msgBox->exec() == QMessageBox::Cancel) {
            res = false;
        } else {
            res = true;
        }

        delete msgBox;
        mutex.unlock();
    }
    return res;
}

bool MyMsgBox::result()
{
    mutex.lock();
    mutex.unlock();
    return res;
}
