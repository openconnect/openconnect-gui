#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QSettings>
#include <QMutex>

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

    ~MainWindow();

private slots:
    void on_disconnectBtn_clicked();

    void on_connectBtn_clicked();

private:
    class VpnInfo *vpninfo;
    Ui::MainWindow *ui;
    QSettings *settings;
    QMutex progress_mutex;
};

#endif // MAINWINDOW_H
