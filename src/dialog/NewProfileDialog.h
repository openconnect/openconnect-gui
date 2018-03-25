#pragma once

#include <QDialog>

class QAbstractButton;
namespace Ui {
class NewProfileDialog;
}

class NewProfileDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewProfileDialog(QWidget* parent = 0);
    ~NewProfileDialog();

    QString getNewProfileName() const;

signals:
    void connect();

protected:
    void changeEvent(QEvent* e);

private slots:
    void on_checkBoxCustomize_toggled(bool checked);
    void on_lineEditName_textChanged(const QString&);
    void on_lineEditGateway_textChanged(const QString& text);

    void on_buttonBox_clicked(QAbstractButton* button);
    void on_buttonBox_accepted();

private:
    void updateButtons();

    Ui::NewProfileDialog* ui;
};
