
#include <QMessageBox>
#include <QDesktopServices>
#include "FileBrowserDialog.h"
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

	}

	void FileBrowserDialog::homepageClicked(bool checked) {
	}

	void FileBrowserDialog::closeClicked(bool checked)
	{
		close();
	}

	void FileBrowserDialog::setFolders(const std::vector<Folder>& folders) {
		_folders = folders;
/*
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

			auto size = new QStandardItem(QString("%1").arg(file.Size));
			size->setEditable(false);

			auto modified = new QStandardItem(QString("%1").arg(file.ModTime));
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
		ui->fileView->resizeColumnToContents(0);
		ui->fileView->resizeColumnToContents(1);
		*/
	}

	std::string FileBrowserDialog::humanSize(uint64_t bytes)
	{
		char *suffix[] = {"B", "KB", "MB", "GB", "TB"};
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
		ui->fileView->resizeColumnToContents(0);
		ui->fileView->resizeColumnToContents(1);
	}

	void FileBrowserDialog::setCurrentFolder(const std::string &currentFolder) {
		ui->currentFolderLabel->setText(QString("Current folder: %1").arg(currentFolder.c_str()));
	}
}