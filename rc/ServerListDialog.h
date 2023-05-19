#ifndef SERVERLISTDIALOG_H
#define SERVERLISTDIALOG_H

#include <QDialog>

namespace Ui {
class ServerListDialog;
}

class ServerListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerListDialog(QWidget *parent = nullptr);
    ~ServerListDialog();

private:
    Ui::ServerListDialog *ui;
};

#endif // SERVERLISTDIALOG_H
