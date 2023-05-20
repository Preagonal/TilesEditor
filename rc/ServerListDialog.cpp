
#include <QMessageBox>
#include <QDesktopServices>
#include "ServerListDialog.h"
#include "ui_ServerListDialog.h"
#include "RCConnection.h"

namespace TilesEditor::RC {
	ServerListDialog::ServerListDialog(QWidget *parent) :
			QDialog(parent),
			ui(new Ui::ServerListDialog) {
		ui->setupUi(this);

		connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ServerListDialog::okClicked);
		connect(ui->treeView, &QTreeView::doubleClicked, this, &ServerListDialog::doubleClicked);

		connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ServerListDialog::cancelClicked);

		connect(ui->homepageButton, &QPushButton::clicked, this, &ServerListDialog::homepageClicked);

		this->setWindowTitle("Graal Servers");
	}

	ServerListDialog::~ServerListDialog() {
		delete ui;
	}

	void ServerListDialog::okClicked()
	{
		/*
		QSettings settings("settings.ini", QSettings::IniFormat);
		settings.setValue("account",ui.accountEdit->text());
		settings.setValue("password",ui.passwordEdit->text());
		settings.setValue("nickname",ui.nickEdit->text());

		connection->setAccount(ui.accountEdit->text().toStdString());
		connection->setPassword(ui.passwordEdit->text().toStdString());
		connection->setNickname(ui.nickEdit->text().toStdString());
*/
		RCConnection* connection = RCConnection::getInstance();

		auto server = _servers[selected];
		if (connection->connectServer(server)) {
			hide();
		}
	}

	void ServerListDialog::doubleClicked(const QModelIndex &index) {
		okClicked();
	}

	void ServerListDialog::activated(const QItemSelection& selectedRow, const QItemSelection& deselectedRow) {
		QModelIndexList selectedIndexes = selectedRow.indexes();
		if (!selectedIndexes.isEmpty()) {
			const QModelIndex& index = selectedIndexes.first();
			// Get the index for column 0 of the selected index
			QModelIndex column0Index = index.sibling(index.row(), 0);

			selected = atoi(column0Index.data(Qt::DisplayRole).value<QString>().toStdString().c_str());
		}

		auto server = _servers[selected];

		ui->languageEdit->setText(server.Language.c_str());
		ui->versionEdit->setText(server.Version.c_str());
		ui->descriptionEdit->setText(server.Description.c_str());
		ui->homepageEdit->setText(server.Url.c_str());
	}

	void ServerListDialog::homepageClicked(bool checked) {
		QDesktopServices::openUrl(QUrl(ui->homepageEdit->text()));
	}

	void ServerListDialog::cancelClicked()
	{
		close();
	}

	void ServerListDialog::setServers(const std::vector<Server>& servers) {
		_servers = servers;

		auto model = new QStandardItemModel;

		QStandardItem * root = model->invisibleRootItem();

		QList<QStandardItem*> rowItems;

		int i = 0;
		for (const auto & server : _servers) {
			auto id = new QStandardItem(QString("%1").arg(i));

			std::string typeStr;

			switch (server.Type) {
				case ServerType::_3D:
					typeStr = ":/MainWindow/icons/fugue/burn.png";
					break;
				case ServerType::Hidden:
					typeStr = ":/MainWindow/icons/fugue/construction.png";
					break;
				case ServerType::Gold:
					typeStr = ":/MainWindow/icons/fugue/crown.png";
					break;
				case ServerType::Classic:
					typeStr = ":/MainWindow/icons/fugue/crown-silver.png";
					break;
				case ServerType::Hosted:
					typeStr = ":/MainWindow/icons/fugue/crown-bronze.png";
					break;
				default:
					typeStr = "";
			}

			auto type = new QStandardItem(QPixmap(typeStr.c_str()), QString("%1").arg(server.Type));
			type->setEditable(false);
			type->setForeground(QBrush(Qt::GlobalColor::black, Qt::BrushStyle::NoBrush));

			auto name = new QStandardItem(QString("%1").arg(server.Name.c_str()));
			name->setTextAlignment(Qt::AlignLeft);
			name->setEditable(false);

			auto players = new QStandardItem(QString("%1").arg(server.PlayerCount.c_str()));
			players->setEditable(false);

			rowItems << id << type << name << players;
			root->appendRow(rowItems);
			rowItems.clear();
			i++;
		}

		//model->setHeaderData(0, Qt::Horizontal, tr(""), Qt::DisplayRole);
		model->setHeaderData(1, Qt::Horizontal, tr("Type"), Qt::DisplayRole);
		model->setHeaderData(2, Qt::Horizontal, tr("Name"), Qt::DisplayRole);
		model->setHeaderData(3, Qt::Horizontal, tr("Players"), Qt::DisplayRole);

		ui->treeView->setModel(model);
		connect(ui->treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ServerListDialog::activated);
		ui->treeView->setItemDelegate(delegate);
		ui->treeView->setColumnHidden(0, true);
		ui->treeView->resizeColumnToContents(0);
		ui->treeView->resizeColumnToContents(1);
	}
}