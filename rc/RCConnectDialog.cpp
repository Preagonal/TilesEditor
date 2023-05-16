#include "RCConnectDialog.h"

namespace TilesEditor::RC
{
	RCConnectDialog::RCConnectDialog(QWidget *parent) :
			QDialog(parent)
	{
		ui.setupUi(this);

		this->setFixedSize(this->size());
	}

	RCConnectDialog::~RCConnectDialog()
	= default;
}