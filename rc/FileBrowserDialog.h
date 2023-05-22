#ifndef FILEBROWSERDIALOG_H
#define FILEBROWSERDIALOG_H

#include <QDialog>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTreeWidgetItem>
#include <QTime>
#include <QStyledItemDelegate>
#include "Server.h"

namespace Ui {
	class FileBrowserDialog;
}

namespace TilesEditor::RC {
	class FileBrowserDialog : public QDialog {
	Q_OBJECT

	public:
		explicit FileBrowserDialog(QWidget *parent = nullptr);

		~FileBrowserDialog() override;

		void setFiles(const std::vector<File>& files);
		void setFolders(const std::vector<Folder>& folders);
		void setCurrentFolder(const std::string& currentFolder);
		void addMessage(const std::string& message);

	private:
		void okClicked();
		void doubleClicked(const QModelIndex &index);
		void activated(const QItemSelection& selectedRow, const QItemSelection& deselectedRow);
		void closeClicked(bool checked = false);
		void homepageClicked(bool checked = false);

		void buildTree(const std::vector<std::string>& strings, QStandardItem* parentItem);
		QStandardItem* findChildItem(const std::string& name, QStandardItem* parentItem);
		QStandardItem* getItem(const QModelIndex& index) const;
		QString getItemPath(const QModelIndex& index) const;

		Ui::FileBrowserDialog *ui;
		std::vector<File> _files;
		std::vector<Folder> _folders;
		uint32_t selected = 0;
		uint32_t folderIter = 0;
		QStandardItem* rootItem = nullptr;
		QStandardItemModel* model;

		static std::string humanSize(uint64_t bytes);
	};
}
#endif // FILEBROWSERDIALOG_H
