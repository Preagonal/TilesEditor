#include "ServerListDialog.h"
#include "ui_ServerListDialog.h"

ServerListDialog::ServerListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerListDialog)
{
    ui->setupUi(this);
}

ServerListDialog::~ServerListDialog()
{
    delete ui;
}
