
#ifndef TILESEDITOR_RCCONNECTION_H
#define TILESEDITOR_RCCONNECTION_H

#include <bits/stdc++.h>
#include <utility>

#include <CString.h>
#include <CSocket.h>
#include <CEncryption.h>
#include <CFileQueue.h>

#include "ServerListDialog.h"
#include "FileBrowserDialog.h"

using namespace std;
namespace TilesEditor::RC {
	enum
	{
		LI_SVRLIST			= 0,
		LI_NULL				= 1,
		LI_STATUS			= 2,
		LI_SITEURL			= 3,
		LI_ERROR			= 4,
		LI_UPGURL			= 5,
		LI_GRSECURELOGIN	= 223
	};

	class RCConnection : public CSocketStub {
		protected:
			// Packet functions.
			bool parsePacket(CString& pPacket);
			void decryptPacket(CString& pPacket);

			// Socket Variables
			bool nextIsRaw;
			int rawPacketSize;
			CFileQueue _fileQueue;
			CString readBuffer;
			CSocket sock;
			std::chrono::high_resolution_clock::time_point lastData, lastTimer;
			time_t nextConnectionAttempt{};
			uint8_t connectionAttempts{};

			std::map<std::string, int> serverListCount;
			std::string _serverLocalIp;
			std::string _serverRemoteIp;

		private:
			std::vector<File> _files;
			std::vector<Folder> _folders;
			string _account, _password, _nickname, _currentFolder;
			ServerListDialog serverList;
			static RCConnection *instancePtr;

			// Incoming message parsing functions
			static bool created;
			static void createFunctions(bool listServerConnection = true);


			// Encryption
			unsigned char key = 0;
			CEncryption in_codec;

			bool listServerConnection = true;

			RCConnection();

		public:
			// Required by CSocketStub.
			bool onRecv() override;
			bool onSend() override;
			bool onRegister() override			{ return true; }
			void onUnregister() override;
			SOCKET getSocketHandle() override	{ return sock.getHandle(); }
			bool canRecv() override;
			bool canSend() override				{ return _fileQueue.canSend(); }

			RCConnection(const RCConnection &obj)
			= delete;

			void operator=(const RCConnection &) = delete;

			static RCConnection *getInstance() {
				if (instancePtr == nullptr)
					instancePtr = new RCConnection();

				return instancePtr;
			}

			static void mainLoop();

			bool doTimedEvents();

			// Socket-Control Functions
			FileBrowserDialog fileBrowser;
			CSocketManager sockManager;
			[[nodiscard]] bool getConnected() const;
			bool doRecv();
			bool connectServer(Server server = {});
			CSocket& getSocket()					{ return sock; }
			void sendPacket(CString& pPacket, bool sendNow = false);

			void setAccount(string account) {
				_account = std::move(account);
			}

			void setPassword(string password) {
				_password = std::move(password);
			}

			void setNickname(string nickname) {
				_nickname = std::move(nickname);
			}

			void connect();

			void msgNULL(CString& pPacket);

			// Listserver Packets
			void msgSTATUS(CString& pPacket);
			void msgUPGURL(CString& pPacket);
			void msgERROR(CString& pPacket);
			void msgSVRLIST(CString& pPacket);

			// RC Packets
			void msgSIGNATURE(CString& pPacket);
			void msgRC_CHAT(CString& pPacket);
			void msgDISCMESSAGE(CString& pPacket);
			void msgRC_FILEBROWSER_DIR(CString& pPacket);
			void msgRC_FILEBROWSER_DIRLIST(CString& pPacket);
			void msgRC_FILEBROWSER_MESSAGE(CString& pPacket);
	};
}
#endif //TILESEDITOR_RCCONNECTION_H