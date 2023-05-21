
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

		connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ServerListDialog::cancelClicked);

		connect(ui->homepageButton, &QPushButton::clicked, this, &ServerListDialog::homepageClicked);
*/
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

	void FileBrowserDialog::cancelClicked()
	{
		close();
	}

	void FileBrowserDialog::setFolders(const std::vector<Folder>& dirs) {

	}

	void FileBrowserDialog::setFiles(const std::vector<File>& files) {

	}

	void FileBrowserDialog::setCurrentFolder(const std::string &currentFolder) {
		ui->currentFolderLabel->setText(QString("Current folder: %1").arg(currentFolder.c_str()));
	}
}