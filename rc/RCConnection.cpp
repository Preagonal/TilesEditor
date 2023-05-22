#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-convert-member-functions-to-static"
#include <QSettings>
#include <IEnums.h>
#include <QMessageBox>
#include "RCConnection.h"

namespace TilesEditor::RC {
	/*
		Pointer-Functions for Packets
	*/
	RCConnection *RCConnection::instancePtr = nullptr;

	bool RCConnection::created = false;
	typedef void (RCConnection::*TSLSock)(CString&);
	std::vector<TSLSock> TSLFunc(256, &RCConnection::msgNULL);

	void RCConnection::createFunctions(bool listServerConnection)
	{
		if (RCConnection::created)
			return;

		// now set non-nulls
		if (listServerConnection) {
			TSLFunc[LI_STATUS] = &RCConnection::msgSTATUS;
			TSLFunc[LI_SITEURL] = &RCConnection::msgSTATUS;
			TSLFunc[LI_UPGURL] = &RCConnection::msgUPGURL;
			TSLFunc[LI_ERROR] = &RCConnection::msgERROR;
			TSLFunc[LI_SVRLIST] = &RCConnection::msgSVRLIST;
		} else {
			TSLFunc[PLO_DISCMESSAGE] = &RCConnection::msgDISCMESSAGE;
			TSLFunc[PLO_RC_FILEBROWSER_DIRLIST] = &RCConnection::msgRC_FILEBROWSER_DIRLIST;
			TSLFunc[PLO_RC_FILEBROWSER_DIR] = &RCConnection::msgRC_FILEBROWSER_DIR;
			TSLFunc[PLO_RC_FILEBROWSER_MESSAGE] = &RCConnection::msgRC_FILEBROWSER_MESSAGE;
			TSLFunc[PLO_RC_CHAT] = &RCConnection::msgRC_CHAT;
			TSLFunc[PLO_SIGNATURE] = &RCConnection::msgSIGNATURE;
		}

		// Finished
		RCConnection::created = true;
	}

	/*
		Socket-Control Functions
	*/
	bool RCConnection::getConnected() const
	{
		return (sock.getState() == SOCKET_STATE_CONNECTED);
	}

	bool RCConnection::onRecv()
	{
		// Grab the data from the socket and put it into our receive buffer.
		unsigned int size = 0;
		char* data = sock.getData(&size);
		if (size != 0)
			readBuffer.write(data, size);
		else if (sock.getState() == SOCKET_STATE_DISCONNECTED)
			return false;

		doRecv();

		return true;
	}

	bool RCConnection::onSend()
	{
		_fileQueue.sendCompress();
		return true;
	}

	bool RCConnection::canRecv()
	{
		if (sock.getState() == SOCKET_STATE_DISCONNECTED) return false;
		return true;
	}

	void RCConnection::onUnregister()
	{
		printf(":: %s - Disconnected.\n", sock.getDescription());
	}

	bool RCConnection::doRecv()
	{
		if (!getConnected())
			return false;

		// definitions
		CString unBuffer;

		// parse data
		readBuffer.setRead(0);
		while (readBuffer.length() > 1)
		{
			// New data.
			lastData = std::chrono::high_resolution_clock::now();

			// packet length
			auto len = (unsigned short)readBuffer.readShort();
			if ((unsigned int)len > (unsigned int)readBuffer.length() - 2)
				break;

			// decompress packet
			unBuffer = readBuffer.readChars(len);
			readBuffer.removeI(0, len + 2);

			// decrypt packet
			switch (in_codec.getGen())
			{
				// Gen 1 is not encrypted or compressed.
				case ENCRYPT_GEN_1:
					break;

				// Gen 2 and 3 are zlib compressed.  Gen 3 encrypts individual packets
				// Uncompress so we can properly decrypt later on.
				case ENCRYPT_GEN_2:
				case ENCRYPT_GEN_3:
					unBuffer.zuncompressI();
					break;

				// Gen 4 and up encrypt the whole combined and compressed packet.
				// Decrypt and decompress.
				default:
					decryptPacket(unBuffer);
					break;
			}

			// well theres your buffer
			if (!parsePacket(unBuffer))
				return false;
		}

		sockManager.updateSingle(this, false, true);

		return getConnected();
	}

	void RCConnection::decryptPacket(CString& pPacket)
	{
		// Version 1.41 - 2.18 encryption
		// Was already decompressed so just decrypt the packet.
		if (in_codec.getGen() == ENCRYPT_GEN_3)
		{
			in_codec.decrypt(pPacket);
		}

		// Version 2.19+ encryption.
		// Encryption happens before compression and depends on the compression used so
		// first decrypt and then decompress.
		if (in_codec.getGen() == ENCRYPT_GEN_4)
		{
			// Decrypt the packet.
			in_codec.limitFromType(COMPRESS_BZ2);
			in_codec.decrypt(pPacket);

			// Uncompress packet.
			pPacket.bzuncompressI();
		}
		else if (in_codec.getGen() >= ENCRYPT_GEN_5)
		{
			// Find the compression type and remove it.
			int pType = pPacket.readChar();
			pPacket.removeI(0, 1);

			// Decrypt the packet.
			in_codec.limitFromType(pType);		// Encryption is partially related to compression.
			in_codec.decrypt(pPacket);

			// Uncompress packet
			if (pType == COMPRESS_ZLIB)
				pPacket.zuncompressI();
			else if (pType == COMPRESS_BZ2)
				pPacket.bzuncompressI();
			else if (pType != COMPRESS_UNCOMPRESSED)
				printf("** [ERROR] Client gave incorrect packet compression type! [%d]\n", pType);
		}
	}

	void RCConnection::sendPacket(CString& pPacket, bool sendNow)
	{
		// empty buffer?
		if (pPacket.isEmpty())
			return;

		// append '\n'
		if (pPacket[pPacket.length()-1] != '\n')
			pPacket.writeChar('\n');

		// append buffer
		_fileQueue.addPacket(pPacket);

		// send buffer now?
		if (sendNow)
			_fileQueue.sendCompress();
	}

	void RCConnection::mainLoop() {
		// Current time
		auto currentTimer = std::chrono::high_resolution_clock::now();

		TilesEditor::RC::RCConnection *connection = TilesEditor::RC::RCConnection::getInstance();
		connection->sockManager.update(0, 5000);		// 5ms

		// Every second, do some events.
		auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
				currentTimer - connection->lastTimer);
		if (time_diff.count() >= 1000) {
			connection->lastTimer = currentTimer;

			connection->doTimedEvents();
		}
	}

	// Called every second by TServer
	bool RCConnection::doTimedEvents()
	{
		lastTimer = std::chrono::high_resolution_clock::now();

		bool isConnected = getConnected();

		if (!isConnected)
		{
			// Reconnect to the listserver, with connection backoff to prevent a flood of connections
			/*
			if (difftime(lastTimer, nextConnectionAttempt) >= 0)
			{
				if (!connectServer())
				{
					if (connectionAttempts < 8)
						connectionAttempts += 1;

					auto waitTime = std::min(uint32_t(std::pow(2u, connectionAttempts)), 300u);
					nextConnectionAttempt = lastTimer + waitTime + (rand() % 5);
				}
				else connectionAttempts = 0;
			}
			*/
		}

		return true;
	}

	bool RCConnection::connectServer(Server server)
	{
		QSettings settings("settings.ini", QSettings::IniFormat);

		listServerConnection = (server.ServerIp.empty() && server.ServerPort.empty());

		RCConnection::created = false;
		RCConnection::createFunctions(listServerConnection);

		if (getConnected()) {
			//return true;
			sock.disconnect();
		}

		std::string serverIp, serverPort;

		if (listServerConnection) {
			serverIp = settings.value("listip","").toString().toStdString();
			serverPort = settings.value("listport","").toString().toStdString();
		} else {
			serverIp = server.ServerIp;
			serverPort = server.ServerPort;
		}

		printf(":: Initializing %s socket.\n", sock.getDescription());

		// Initialize the socket
		if (sock.init(serverIp.c_str(), serverPort.c_str()) != 0)
		{
			printf(":: [Error] Could not initialize %s socket.\n", sock.getDescription());
			return false;
		}

		// Connect to Server
		if (sock.connect() != 0)
		{
			printf(":: [Error] Could not connect %s socket.\n",sock.getDescription());
			return false;
		}

		sockManager.registerSocket((CSocketStub*)this);
		printf(":: %s - Connected.\n", sock.getDescription());


		// TODO(joey): Some packets were being queued up from the server before we were connected, and would spam the serverlist
		// upon connection. Clearing the outgoing buffer upon connection
		_fileQueue.clearBuffers();
		_fileQueue.setCodec(ENCRYPT_GEN_2, 0);

		std::string versionString = "GSERV025";

		if (listServerConnection) {
			sendPacket(CString() >> (char) 5 >> (char) key << versionString.c_str(), true);
			_fileQueue.setCodec(ENCRYPT_GEN_5, key);
			in_codec.setGen(ENCRYPT_GEN_5);
			in_codec.reset(key);

			sendPacket(CString() >> (char) 1 >> (char) _account.size() << _account >> (char) _password.size() << _password);
		} else {
			CString keyPacket = CString () >> (char)6 >> (char)key << versionString.c_str() >> (char)_account.size() << _account >> (char)_password.size() << _password;// + new CString ( "win" ).Tokenize () + ",";
			sendPacket(keyPacket, true);
			_fileQueue.setCodec(ENCRYPT_GEN_5, key);
			in_codec.setGen(ENCRYPT_GEN_5);
			in_codec.reset(key);

			sendPacket(CString() >> (char)PLI_PLAYERPROPS >> (char)0 >> (char)_nickname.size() << _nickname);
		}

		// Return Connection-Status
		return getConnected();
	}

	/*
		Packet-Functions
	*/
	bool RCConnection::parsePacket(CString& pPacket)
	{
		while (pPacket.bytesLeft() > 0)
		{
			CString curPacket;
			if (nextIsRaw)
			{
				nextIsRaw = false;
				curPacket = pPacket.readChars(rawPacketSize);
			}
			else curPacket = pPacket.readString("\n");

			// read id & packet
			unsigned char id = curPacket.readGUChar();

			// valid packet, call function
			(*this.*TSLFunc[id])(curPacket);
		}

		return true;
	}

	void RCConnection::msgNULL(CString& pPacket)
	{
		pPacket.setRead(0);
		qDebug() << QString(":: Unknown Serverlist Packet: %1 (%2)").arg(pPacket.readGUChar()).arg(pPacket.text()+1) << "\n";
	}

	void RCConnection::msgSTATUS(CString& pPacket)
	{
		auto status = pPacket.readString("\n");
		qDebug() << status.text() << "\n";
	}

	void RCConnection::msgSIGNATURE(CString& pPacket)
	{
		pPacket.readString("\n");
		sendPacket(CString() >> (char)PLI_RC_FILEBROWSER_START);
	}

	void RCConnection::msgRC_CHAT(CString& pPacket)
	{
		auto status = pPacket.readString("\n");
		qDebug() << status.text() << "\n";

	}

	void RCConnection::msgRC_FILEBROWSER_MESSAGE(CString& pPacket)
	{
		auto message = pPacket.readString("\n");
		fileBrowser.addMessage(message.text());
	}

	void RCConnection::msgRC_FILEBROWSER_DIR(CString& pPacket)
	{
		auto folder = pPacket.readChars(pPacket.readGUChar());
		qDebug() << folder.text() << "\n";


		_files.clear();

		_currentFolder = folder.text();

		fileBrowser.setCurrentFolder(_currentFolder);

		while (pPacket.bytesLeft() > 0) {
			pPacket.readGUChar(); // always " " ??
			auto file = pPacket.readChars(pPacket.readGUChar());

			_files.push_back(
				{
					.Name = file.readChars(file.readGUChar()).text(),
					.Rights = file.readChars(file.readGUChar()).text(),
					.Size = file.readGInt5(),
					.ModTime = file.readGInt5()
				}
			);
		}
		fileBrowser.setFiles(_files);
		fileBrowser.open();
	}

	void RCConnection::msgRC_FILEBROWSER_DIRLIST(CString& pPacket)
	{
		auto folderList = pPacket.readString("\n").guntokenizeI().tokenize("\n");

		_folders.clear();
		for (const auto & folder : folderList) {
			auto folderColumns = folder.tokenize(" ");
			_folders.push_back(
				{
					.Name = folderColumns[1].text(),
					.Rights = folderColumns[0].text()
				}
			);
		}

		fileBrowser.setFolders(_folders);
	}

	void RCConnection::msgDISCMESSAGE(CString& pPacket)
	{
		auto status = pPacket.readString("\n");
		qDebug() << status.text() << "\n";
		QMessageBox::information(nullptr, "Serverlist Status", status.text(), QMessageBox::Ok);
		sock.disconnect();
	}

	void RCConnection::msgUPGURL(CString& pPacket)
	{
		auto status = pPacket.readString("\n");
		qDebug() << status.text() << "\n";
		//QMessageBox::information(nullptr, "Serverlist Status", status.text(), QMessageBox::Ok);
		sock.disconnect();
	}

	void RCConnection::msgERROR(CString& pPacket)
	{
		auto status = pPacket.readString("\n");
		qDebug() << status.text() << "\n";
		//QMessageBox::information(nullptr, "Serverlist Status", status.text(), QMessageBox::Ok);
		sock.disconnect();
	}

	void RCConnection::msgSVRLIST(CString& pPacket)
	{
		auto numServers = pPacket.readGUChar();

		std::vector<Server> servers;

		for (int i = 0; i < numServers; i++) {
			pPacket.readGUChar(); // Number of fields. Always 8
			std::string name = pPacket.readChars(pPacket.readGUChar()).text();
			std::string typeStr = name.substr(0, 2);
			ServerType type = ServerType::Classic;

			if (typeStr == "3 ")
				type = ServerType::_3D;
			else if (typeStr == "P ")
				type = ServerType::Gold;
			else if (typeStr == "H ")
				type = ServerType::Hosted;
			else if (typeStr == "U ")
				type = ServerType::Hidden;

			if (type > ServerType::Classic)
				name = name.substr(2);

			servers.push_back(
				{
					.Name = name,
					.Language = pPacket.readChars(pPacket.readGUChar()).text(),
					.Description = pPacket.readChars(pPacket.readGUChar()).text(),
					.Url = pPacket.readChars(pPacket.readGUChar()).text(),
					.Version = pPacket.readChars(pPacket.readGUChar()).text(),
					.PlayerCount = pPacket.readChars(pPacket.readGUChar()).text(),
					.ServerIp = pPacket.readChars(pPacket.readGUChar()).text(),
					.ServerPort = pPacket.readChars(pPacket.readGUChar()).text(),
					.Type = type
				}
		  	);
		}

		serverList.setServers(servers);
		serverList.open();
	}

	void RCConnection::connect() {

	}

	RCConnection::RCConnection() : _fileQueue(&sock), nextIsRaw(false), rawPacketSize(0) {
		// Create Functions
		if (!RCConnection::created)
			RCConnection::createFunctions();
	}
}
#pragma clang diagnostic pop