#include "NewProfileDialog.h"
#include "ui_NewProfileDialog.h"

#include "server_storage.h"

#include <QPushButton>
#include <QSettings>
#include <QUrl>

#include <memory>

NewProfileDialog::NewProfileDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::NewProfileDialog)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::SaveAll)->setText(tr("Save && Connect"));
    ui->buttonBox->button(QDialogButtonBox::SaveAll)->setDefault(true);

    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::SaveAll)->setEnabled(false);
}

NewProfileDialog::~NewProfileDialog()
{
    delete ui;
}

QString NewProfileDialog::getNewProfileName() const
{
    return ui->lineEditName->text();
}

void NewProfileDialog::changeEvent(QEvent* e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void NewProfileDialog::on_checkBoxCustomize_toggled(bool checked)
{
    if (checked == false) {
        QUrl url(ui->lineEditGateway->text());
        if (url.isValid()) {
            ui->lineEditName->setText(url.host());
        }

        ui->lineEditGateway->setFocus();
    } else {
        ui->lineEditName->setFocus();
    }
}

void NewProfileDialog::on_lineEditName_textChanged(const QString&)
{
    updateButtons();
}

void NewProfileDialog::on_lineEditGateway_textChanged(const QString& text)
{
    QUrl url(text, QUrl::StrictMode);
    if (ui->checkBoxCustomize->isChecked() == false && (url.isValid() || text.isEmpty())) {
        ui->lineEditName->setText(url.host());
    }

    updateButtons();
}

#define PREFIX "server:"
void NewProfileDialog::updateButtons()
{
    bool enableButtons{ false };
    if (ui->lineEditName->text().isEmpty() == false && ui->lineEditGateway->text().isEmpty() == false) {

        enableButtons = true;

        // TODO: refactor this too :/
        QSettings settings;
        for (const auto& key : settings.allKeys()) {
            if (key.startsWith(PREFIX) && key.endsWith("/server")) {
                QString str{ key };
                str.remove(0, sizeof(PREFIX) - 1); /* remove prefix */
                str.remove(str.size() - 7, 7); /* remove /server suffix */
                if (str == ui->lineEditName->text()) {
                    enableButtons = false;
                    break;
                }
            }
        }
    }

    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(enableButtons);
    ui->buttonBox->button(QDialogButtonBox::SaveAll)->setEnabled(enableButtons);
}

void NewProfileDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::SaveAll) {
        emit connect();
    }
}

void NewProfileDialog::on_buttonBox_accepted()
{
    auto ss{ std::make_unique<StoredServer>() };
    ss->set_label(ui->lineEditName->text());
    ss->set_servername(ui->lineEditGateway->text());
    ss->save();

    accept();
}
