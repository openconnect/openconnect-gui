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

#ifndef DIALOGS_H
#define DIALOGS_H

#include <mainwindow.h>
#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>
#include <QMutex>

class MyInputDialog:public QObject {

public:
     MyInputDialog(QWidget *w, QString t1, QString t2, QStringList list) {
     	this->w = w;
     	this->t1 = t1;
     	this->t2 = t2;
     	this->list = list;
        this->have_list = true;
        mutex.lock();
        this->moveToThread( QApplication::instance()->thread());
     };
     MyInputDialog(QWidget *w, QString t1, QString t2, QLineEdit::EchoMode type) {
     	this->w = w;
     	this->t1 = t1;
     	this->t2 = t2;
        have_list = false;
        this->type = type;
        mutex.lock();
        this->moveToThread( QApplication::instance()->thread());
     };
     ~MyInputDialog() {
     	mutex.tryLock();
     	mutex.unlock();
     }
     void show() {
     	QCoreApplication::postEvent(this, new QEvent( QEvent::User));
     }
     virtual bool event(QEvent * ev) {
        res = false;
        if (ev->type() == QEvent::User) {
            if (this->have_list)
                text = QInputDialog::getItem(w, t1, t2, list, 0, true, &res);
            else
                text = QInputDialog::getText(w, t1, t2, type, QString(), &res);
            mutex.unlock();
        }
        return res;
    }

    bool result(QString & text) {
        mutex.lock();
        mutex.unlock();
        text = this->text;
        return res;
    };

private:
     QString text;
     bool res;
     QMutex mutex;
     bool have_list;
     QWidget *w;
     QString t1;
     QString t2;
     QStringList list;
     QLineEdit::EchoMode type;
};

class MyMsgBox:public QObject {

public:
     MyMsgBox(QWidget *w, QString t1, QString t2, QString oktxt) {
     	this->w = w;
     	this->t1 = t1;
     	this->t2 = t2;
     	this->oktxt = oktxt;
        mutex.lock();
        this->moveToThread( QApplication::instance()->thread());
     };
     ~MyMsgBox() {
        mutex.tryLock();
        mutex.unlock();
     }
     void show() {
     	QCoreApplication::postEvent(this, new QEvent( QEvent::User));
     }
     virtual bool event(QEvent * ev) {
        res = false;
        if (ev->type() == QEvent::User) {
            QMessageBox *msgBox = new QMessageBox(w);
            int ret;

            msgBox->setText(t1);
            msgBox->setInformativeText(t2);
            msgBox->setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
            msgBox->setDefaultButton(QMessageBox::Cancel);
            msgBox->setButtonText(QMessageBox::Ok, oktxt);

            ret = msgBox->exec();
            if (ret == QMessageBox::Cancel)
                res = false;
            else
                res = true;

            delete msgBox;
            mutex.unlock();
        }
        return res;
    }
    
    bool result() {
        mutex.lock();
        mutex.unlock();
        return res;
    };

private:
     bool res;
     QMutex mutex;
     QWidget *w;
     QString t1;
     QString t2;
     QString oktxt;
};

#endif // DIALOGS_H
