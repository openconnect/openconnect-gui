#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    void updateProgressBar(const char *str, int value);
    void enableDisconnect(bool);
    ~MainWindow();

private slots:
    void on_disconnectBtn_clicked();

    void on_connectBtn_clicked();

private:
    Ui::MainWindow *ui;
    int cmd_fd;
    QMutex progress_mutex;
};

#endif // MAINWINDOW_H
