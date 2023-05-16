#pragma once

#include <QDialog>
#include "ui_AboutDialog.h"

namespace TilesEditor
{
	class AboutDialog : public QDialog
	{
		Q_OBJECT

	public:
		explicit AboutDialog(QWidget* parent = nullptr);
		~AboutDialog() override;

	private:
		Ui::AboutDialogClass ui{};
	};
}
