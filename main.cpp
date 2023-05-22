#include <iostream>

#include "MainWindow.h"
#include "RCConnection.h"
#include "RCConnectDialog.h"
#include <QtWidgets/QApplication>

static bool rcMode = false;

void printHelp(const char* pname)
{
	//serverlog.out("%s %s version %s\n", APP_VENDOR, APP_NAME, APP_VERSION);
	//serverlog.out("Programmed by %s.\n\n", APP_CREDITS);
	std::cout << "USAGE: " << pname << " [options]\n\n";
	std::cout << "Commands:\n\n";
	std::cout << " -h, --help\t\tPrints out this help text.\n";
	std::cout << " --rc, --remotecontrol DIR\tOverride the servers.txt by specifying which server directory to use.\n";

	std::cout << std::endl;
}

bool parseArgs(int argc, char* argv[])
{
	std::vector<CString> args;

	auto test_for_end = [&args](auto &&iterator, auto &&end)
	{
		if (iterator == end) {
			//printHelp(args[0].text());
			return true;
		}
		return false;
	};

	rcMode = getenv("RCMODE");

	if (!rcMode) {
		for ( int i = 0; i < argc; ++i )
			args.push_back(CString(argv[i]));

		for ( auto i = args.begin(); i != args.end(); ++i ) {
			if ((*i).find("--") == 0 ) {
				CString key((*i).subString(2));
				if (key == "help") {
					printHelp(args[0].text());
					return true;
				} else {
					if (key == "rc" || key == "remotecontrol")
						rcMode = true;

					if (test_for_end(++i, args.end()))
						return true;
				}
			}
		}
	}

	return false;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	parseArgs(argc, argv);

	// app.setStyle("fusion");
    if (false)
    {
        QPalette darkPalette;
        darkPalette = app.palette();

        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
        darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::Dark, QColor(35, 35, 35));
        darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
        darkPalette.setColor(QPalette::HighlightedText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));

        app.setPalette(darkPalette);
    }

	auto timer = new QTimer();
	timer->start(5);
	QApplication::connect(timer, &QTimer::timeout, &app, QOverload<>::of(&TilesEditor::RC::RCConnection::mainLoop));

	TilesEditor::MainWindow mw;
	TilesEditor::RC::RCConnectDialog rc;

	if (!rcMode)
		mw.show();
	else
		rc.show();

	return QApplication::exec();
}
