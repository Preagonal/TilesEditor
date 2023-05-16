#ifndef TILESEDITOR_RCCONNECTDIALOG_H
#define TILESEDITOR_RCCONNECTDIALOG_H
#pragma once

#include <QDialog>
#include "ui_RCConnectDialog.h"

namespace TilesEditor::RC {
	class RCConnectDialog : public QDialog
	{
		Q_OBJECT

	public:
		explicit RCConnectDialog(QWidget *parent = nullptr);

		~RCConnectDialog() override;

	private:
		Ui::RCConnectDialog ui{};
	};
}

#endif //TILESEDITOR_RCCONNECTDIALOG_H
