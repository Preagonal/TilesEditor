#include <regex>

#include <QMessageBox>
#include <QDesktopServices>
#include <IEnums.h>
#include "FileBrowserDialog.h"
#include "RCConnection.h"
#include "ui_FileBrowserDialog.h"

namespace TilesEditor::RC {
	FileBrowserDialog::FileBrowserDialog(QWidget *parent) :
			QDialog(parent),
			ui(new Ui::FileBrowserDialog) {
		ui->setupUi(this);
/*
		connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ServerListDialog::okClicked);
		connect(ui->treeView, &QTreeView::doubleClicked, this, &ServerListDialog::doubleClicked);

*/

		connect(ui->closeButton, &QPushButton::clicked, this, &FileBrowserDialog::closeClicked);

		this->setWindowTitle("File Browser");
	}

	FileBrowserDialog::~FileBrowserDialog() {
		delete ui;
	}

	void FileBrowserDialog::okClicked()
	{

	}

	void FileBrowserDialog::doubleClicked(const QModelIndex &index) {
		okClicked();
	}

	void FileBrowserDialog::activated(const QItemSelection& selectedRow, const QItemSelection& deselectedRow) {
		QModelIndexList selectedIndexes = selectedRow.indexes();
		Folder foundFolder;
		std::string folderPath;

		if (!selectedIndexes.isEmpty()) {
			const QModelIndex& index = selectedIndexes.first();

			auto newIconData = QVariant::fromValue<QIcon>(QIcon(":/MainWindow/icons/fugue/blue-folder-horizontal-open.png")); // Replace with your new icon path
			model->setData(index, newIconData, Qt::DecorationRole);

			folderPath = getItemPath(index).toStdString();

			//itemPath += "*";
			/*
            qDebug() << "Selected item path: " << QString::fromStdString(itemPath);

			for (const auto& folder : _folders) {
				if (folder.Name == itemPath) {
					foundFolder = folder;
					break;
				}
			}*/
		}

		QModelIndexList deselectedIndexes = deselectedRow.indexes();
		if (!deselectedIndexes.isEmpty()) {
			const QModelIndex& index = deselectedIndexes.first();

			bool isExpanded = ui->directoryView->isExpanded(index);
			if (!isExpanded) {
				QVariant newIconData = QVariant::fromValue<QIcon>(QIcon(":/MainWindow/icons/fugue/blue-folder-horizontal.png")); // Replace with your new icon path
				model->setData(index, newIconData, Qt::DecorationRole);
			}
		}

		if (!folderPath.empty()) {
			RCConnection* connection = RCConnection::getInstance();
			connection->sendPacket(CString() >> (char)PLI_RC_FILEBROWSER_CD << folderPath);
		}

		/*
		auto folder = _folders[selected];

		ui->languageEdit->setText(server.Language.c_str());
		ui->versionEdit->setText(server.Version.c_str());
		ui->descriptionEdit->setText(server.Description.c_str());
		ui->homepageEdit->setText(server.Url.c_str());
		*/
	}

	void FileBrowserDialog::homepageClicked(bool checked) {
	}

	void FileBrowserDialog::closeClicked(bool checked)
	{
		close();
	}

	void FileBrowserDialog::buildTree(const std::vector<std::string> &strings, QStandardItem *parentItem) {
		for (const auto& str : strings) {
			if (str.find('/') != std::string::npos) {
				std::size_t pos = str.find('/');
				std::string dirName = str.substr(0, pos + 1);
				std::string remainingStr = str.substr(pos + 1);

				QStandardItem* dirItem = findChildItem(dirName, parentItem);
				if (!dirItem) {
					dirItem = new QStandardItem(QPixmap(":/MainWindow/icons/fugue/blue-folder-horizontal.png"), QString::fromStdString(dirName));
					parentItem->appendRow(dirItem);
					folderIter++;
				}

				buildTree({ remainingStr }, dirItem);
			} else {
				if (str != "") {
					QStandardItem* fileItem = new QStandardItem(QString::fromStdString(str));
					parentItem->appendRow(fileItem);
					folderIter++;
				}
			}
		}
	}

	QStandardItem* FileBrowserDialog::findChildItem(const std::string &name, QStandardItem *parentItem) {
		for (int i = 0; i < parentItem->rowCount(); ++i) {
			QStandardItem* childItem = parentItem->child(i);
			if (childItem->text() == QString::fromStdString(name))
				return childItem;
		}
		return nullptr;
	}

	QStandardItem* FileBrowserDialog::getItem(const QModelIndex& index) const {
		if (index.isValid()) {
			QStandardItem* item = static_cast<QStandardItem*>(index.internalPointer());
			if (item)
				return item;
		}
		return rootItem;
	}

	QString FileBrowserDialog::getItemPath(const QModelIndex& index) const {
		QStringList pathList;

		QModelIndex currentIndex = index;
		while (currentIndex.isValid()) {
			QString itemName = currentIndex.data(Qt::DisplayRole).toString();
			pathList.prepend(itemName);

			currentIndex = currentIndex.parent();
		}

		QString itemPath = pathList.join("");
		return itemPath;
	}

	void FileBrowserDialog::setFolders(const std::vector<Folder>& folders) {
		_folders = folders;

		open();
		model = new QStandardItemModel;

		rootItem = model->invisibleRootItem();

		folderIter = 0;

		std::vector<std::string> folderStrings;

		for (auto folder : _folders) {
			std::string nameString = std::regex_replace(folder.Name, std::regex("\\*"), "");
			folderStrings.push_back(nameString);
		}

        buildTree(folderStrings, rootItem);

		/*
		//model->setHeaderData(0, Qt::Horizontal, tr(""), Qt::DisplayRole);
		model->setHeaderData(1, Qt::Horizontal, tr("Name"), Qt::DisplayRole);
		model->setHeaderData(2, Qt::Horizontal, tr("Rights"), Qt::DisplayRole);
		model->setHeaderData(3, Qt::Horizontal, tr("Size"), Qt::DisplayRole);
		model->setHeaderData(4, Qt::Horizontal, tr("Modified"), Qt::DisplayRole);
		*/

		ui->directoryView->setModel(model);
		connect(ui->directoryView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FileBrowserDialog::activated);
		//connect(ui->directoryView, &QTreeView::expanded, this, &FileBrowserDialog::directoryExpanded);
		//ui->fileView->setItemDelegate(delegate);
		//ui->directoryView->setColumnHidden(0, true);
		//ui->directoryView->resizeColumnToContents(0);
		//ui->directoryView->resizeColumnToContents(1);

	}

	std::string FileBrowserDialog::humanSize(uint64_t bytes)
	{
		const char *suffix[] = {"B", "KB", "MB", "GB", "TB"};
		char length = sizeof(suffix) / sizeof(suffix[0]);

		int i = 0;
		double dblBytes = bytes;

		if (bytes > 1024) {
			for (i = 0; (bytes / 1024) > 0 && i<length-1; i++, bytes /= 1024)
				dblBytes = bytes / 1024.0;
		}

		static char output[200];
		sprintf(output, "%.02lf %s", dblBytes, suffix[i]);
		return output;
	}

	void FileBrowserDialog::setFiles(const std::vector<File>& files) {
		_files = files;

		auto model = new QStandardItemModel;

		QStandardItem * root = model->invisibleRootItem();

		QList<QStandardItem*> rowItems;

		int i = 0;
		for (const auto & file : _files) {
			auto id = new QStandardItem(QString("%1").arg(i));

			std::string typeStr;

			auto name = new QStandardItem(QString("%1").arg(file.Name.c_str()));
			name->setTextAlignment(Qt::AlignLeft);
			name->setEditable(false);

			auto rights = new QStandardItem(QString("%1").arg(file.Rights.c_str()));
			rights->setEditable(false);

			auto size = new QStandardItem(QString("%1").arg(humanSize(file.Size).c_str()));
			size->setEditable(false);

			auto modified = new QStandardItem(QString("%1").arg(ctime(&file.ModTime)));
			modified->setEditable(false);

			rowItems << id << name << rights << size << modified;
			root->appendRow(rowItems);
			rowItems.clear();
			i++;
		}

		//model->setHeaderData(0, Qt::Horizontal, tr(""), Qt::DisplayRole);
		model->setHeaderData(1, Qt::Horizontal, tr("Name"), Qt::DisplayRole);
		model->setHeaderData(2, Qt::Horizontal, tr("Rights"), Qt::DisplayRole);
		model->setHeaderData(3, Qt::Horizontal, tr("Size"), Qt::DisplayRole);
		model->setHeaderData(4, Qt::Horizontal, tr("Modified"), Qt::DisplayRole);

		ui->fileView->setModel(model);
		connect(ui->fileView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FileBrowserDialog::activated);
		//ui->fileView->setItemDelegate(delegate);
		ui->fileView->setColumnHidden(0, true);
		//ui->fileView->resizeColumnToContents(0);
		ui->fileView->resizeColumnToContents(2);
	}

	void FileBrowserDialog::setCurrentFolder(const std::string &currentFolder) {
		ui->currentFolderLabel->setText(QString("Current folder: %1").arg(currentFolder.c_str()));
	}

	void FileBrowserDialog::addMessage(const std::string &message) {
		ui->textOutput->append(QString("%1\n").arg(message.c_str()));
	}
}