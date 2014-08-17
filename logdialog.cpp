#include "logdialog.h"
#include "ui_logdialog.h"

LogDialog::LogDialog(QStringList items, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
    ui->listWidget->addItems(items);
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::on_pushButton_clicked()
{
    this->close();
}
