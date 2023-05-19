
#ifndef TILESEDITOR_RCCONNECTION_H
#define TILESEDITOR_RCCONNECTION_H

#include <bits/stdc++.h>
#include <utility>

#include <CString.h>
#include <CSocket.h>
#include <CEncryption.h>
#include <CFileQueue.h>

using namespace std;
namespace TilesEditor::RC {
	enum
	{
		PLO_SVRLIST			= 0,
		PLO_NULL			= 1,
		PLO_STATUS			= 2,
		PLO_SITEURL			= 3,
		PLO_ERROR			= 4,
		PLO_UPGURL			= 5,
		PLO_GRSECURELOGIN	= 223
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
			string _account, _password, _nickname;

			static RCConnection *instancePtr;

			// Incoming message parsing functions
			static bool created;
			static void createFunctions();


			// Encryption
			unsigned char key = 0;
			CEncryption in_codec;


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
			CSocketManager sockManager;
			[[nodiscard]] bool getConnected() const;
			bool main();
			bool connectServer();
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

			void msgSTATUS(CString& pPacket);
	};
}
#endif //TILESEDITOR_RCCONNECTION_H
