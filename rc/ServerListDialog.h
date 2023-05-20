#ifndef SERVERLISTDIALOG_H
#define SERVERLISTDIALOG_H

#include <QDialog>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTreeWidgetItem>
#include <QTime>
#include <QStyledItemDelegate>
#include "Server.h"

namespace Ui {
	class ServerListDialog;
}

namespace TilesEditor::RC {
	class IconOnlyDelegate : public QStyledItemDelegate
	{
	public:
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
		{
			// Check if the column index matches the column you want to display only the icon
			if (index.column() == 1)
			{
				// Retrieve the icon for the item
				QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();

				// Calculate the position to draw the icon
				int iconX = option.rect.center().x() - icon.actualSize(option.rect.size()).width() / 2;
				int iconY = option.rect.center().y() - icon.actualSize(option.rect.size()).height() / 2;

				// Draw the icon
				icon.paint(painter, QRect(QPoint(iconX, iconY), icon.actualSize(option.rect.size())), Qt::AlignCenter, QIcon::Normal, QIcon::On);
			}
			else if (index.column() == 0) {

			}
			else
			{
				// For other columns, let the base class handle the default painting
				QStyledItemDelegate::paint(painter, option, index);
			}
		}
	};

	class ServerListDialog : public QDialog {
	Q_OBJECT

	public:
		explicit ServerListDialog(QWidget *parent = nullptr);

		~ServerListDialog() override;

		void setServers(const std::vector<Server>& servers);

	private:
		void okClicked();
		void doubleClicked(const QModelIndex &index);
		void activated(const QItemSelection& selectedRow, const QItemSelection& deselectedRow);
		void cancelClicked();
		void homepageClicked(bool checked = false);

		IconOnlyDelegate* delegate = new IconOnlyDelegate;
		Ui::ServerListDialog *ui;
		std::vector<Server> _servers;
		uint32_t selected = 0;
	};
}
#endif // SERVERLISTDIALOG_H
