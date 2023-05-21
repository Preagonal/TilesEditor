#ifndef TILESEDITOR_SERVER_H
#define TILESEDITOR_SERVER_H

#include <string>

namespace TilesEditor::RC {
	enum ServerType {
		Classic = 0,
		Hosted = 1,
		Gold = 2,
		_3D = 3,
		Hidden = 4,
	};

	typedef struct Server {
		std::string Name;
		std::string Language;
		std::string Description;
		std::string Url;
		std::string Version;
		std::string PlayerCount;
		std::string ServerIp;
		std::string ServerPort;
		ServerType Type;
	} Server;

	typedef struct File {
		std::string Name;
		std::string Rights;
		long long Size;
		time_t ModTime;
	} File;

	typedef struct Dir {
		std::string Name;
		std::string Rights;
	} Folder;
}

#endif //TILESEDITOR_SERVER_H
