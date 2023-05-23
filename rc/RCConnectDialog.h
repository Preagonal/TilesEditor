#ifndef TILESEDITOR_RCCONNECTDIALOG_H
#define TILESEDITOR_RCCONNECTDIALOG_H
#pragma once

#include <QDialog>
#include "ui_RCConnectDialog.h"
#include "MainWindow.h"

namespace TilesEditor::RC {
	class RCConnectDialog : public QDialog
	{
		Q_OBJECT


	public:
		explicit RCConnectDialog(QWidget *parent = nullptr);
		void setMainWindow(TilesEditor::MainWindow* pWindow) { mainWindow = pWindow; };

		~RCConnectDialog() override;

	private:
		Ui::RCConnectDialog ui{};
		TilesEditor::MainWindow* mainWindow;

		void okClicked();
		void cancelClicked();
	};
}

#endif //TILESEDITOR_RCCONNECTDIALOG_H
