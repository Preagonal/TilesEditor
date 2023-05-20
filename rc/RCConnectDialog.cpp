#include <QSettings>
#include "RCConnectDialog.h"
#include "RCConnection.h"

namespace TilesEditor::RC
{
	RCConnectDialog::RCConnectDialog(QWidget *parent) :
			QDialog(parent)
	{
		ui.setupUi(this);
		QSettings settings("settings.ini", QSettings::IniFormat);
		ui.accountEdit->setText(settings.value("account","").toString());
		ui.passwordEdit->setText(settings.value("password","").toString());
		ui.nickEdit->setText(settings.value("nickname","").toString());

		this->setFixedSize(this->size());

		connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &RCConnectDialog::okClicked);
		connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &RCConnectDialog::cancelClicked);
	}

	void RCConnectDialog::okClicked()
	{
		QSettings settings("settings.ini", QSettings::IniFormat);
		settings.setValue("account",ui.accountEdit->text());
		settings.setValue("password",ui.passwordEdit->text());
		settings.setValue("nickname",ui.nickEdit->text());

		RCConnection* connection = RCConnection::getInstance();
		connection->setAccount(ui.accountEdit->text().toStdString());
		connection->setPassword(ui.passwordEdit->text().toStdString());
		connection->setNickname(ui.nickEdit->text().toStdString());

		if (connection->connectServer(TilesEditor::RC::Server())) {
			hide();
		}
	}

	void RCConnectDialog::cancelClicked()
	{
		close();
	}

	RCConnectDialog::~RCConnectDialog()
	= default;
}