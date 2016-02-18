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

#include "MyCertMsgBox.h"

MyCertMsgBox::MyCertMsgBox(QWidget* w, QString t1, QString t2, QString oktxt, QString details)
{
    this->w = w;
    this->t1 = t1;
    this->t2 = t2;
    this->oktxt = oktxt;
    this->details = details;
    mutex.lock();
    this->moveToThread(QApplication::instance()->thread());
}

MyCertMsgBox::~MyCertMsgBox()
{
    mutex.tryLock();
    mutex.unlock();
}

void MyCertMsgBox::show()
{
    QCoreApplication::postEvent(this, new QEvent(QEvent::User));
}

bool MyCertMsgBox::event(QEvent* ev)
{
    res = false;
    if (ev->type() == QEvent::User) {
        QMessageBox* msgBox = new QMessageBox(w);
        int ret;

        msgBox->setText(t1);
        msgBox->setInformativeText(t2);
        msgBox->setStandardButtons(QMessageBox::
                                       Cancel
                                   | QMessageBox::Help | QMessageBox::Ok);
        msgBox->setDefaultButton(QMessageBox::Cancel);
        msgBox->setButtonText(QMessageBox::Ok, oktxt);
        msgBox->setButtonText(QMessageBox::Help, tr("View certificate"));

        do {
            ret = msgBox->exec();
            if (ret == QMessageBox::Help) {
                QMessageBox helpBox;
                helpBox.setTextInteractionFlags(Qt::
                                                    TextSelectableByMouse
                                                | Qt::TextSelectableByKeyboard
                                                | Qt::LinksAccessibleByMouse);
                helpBox.setText(details);
                helpBox.setTextFormat(Qt::PlainText);
                helpBox.setStandardButtons(QMessageBox::Ok);
                helpBox.exec();
            }
        } while (ret == QMessageBox::Help);

        if (ret == QMessageBox::Cancel)
            res = false;
        else
            res = true;

        delete msgBox;
        mutex.unlock();
    }
    return res;
}

bool MyCertMsgBox::result()
{
    mutex.lock();
    mutex.unlock();
    return res;
}
