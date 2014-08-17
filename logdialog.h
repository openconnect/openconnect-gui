#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(QStringList items, QWidget *parent = 0);
    ~LogDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::LogDialog *ui;
};

#endif // LOGDIALOG_H
