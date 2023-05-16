#ifndef MAINWINDOWH
#define MAINWINDOWH

#include <QtWidgets/QMainWindow>
#include <QStringListModel>
#include "ui_MainWindow.h"
#include "TileGroupListModel.h"
#include "ResourceManager.h"
#include "EditorTabWidget.h"

namespace TilesEditor
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT
    public slots:
        void openFile(bool checked);
        void newLevel(bool checked);
        void openLevel(const QString& levelName);
        void changeTabText(const QString& text);
        void setStatusText(const QString& text, int section, int msecs);
        void tabChanged(int index);
        static void aboutClicked(bool checked);
        static void rcClicked(bool checked);
        void closeTabIndexSlot(int index);

    public:
        MainWindow(QWidget* parent = nullptr);
        ~MainWindow();

        void openLevelFilename(const QString& fileName);

    private:
        Ui::MainWindowClass ui{};

        ResourceManager m_resourceManager;
        QStringListModel m_tilesetList;
        TileGroupListModel m_tileGroupsList;

        QLabel* m_statusLeft;
        QLabel* m_statusMiddle;

        //Brings a widget into the specified dock
        void takeWidgetIntoDock(QWidget* dockContainer, QWidget* target);
        void loadTileObjects();
        bool closeTabIndex(int index);
        void closeEvent(QCloseEvent* event) override;
        EditorTabWidget* createNewTab();

    };
}

#endif
