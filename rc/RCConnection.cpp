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

	void RCConnection::createFunctions()
	{
		if (RCConnection::created)
			return;

		// now set non-nulls
		//TSLFunc[PLO_RC_CHAT] = &RCConnection::msgRC_CHAT;
		TSLFunc[PLO_STATUS] = &RCConnection::msgSTATUS;

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

		main();

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


	bool RCConnection::main()
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

		// Every second, do some events.
		auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
				currentTimer - connection->lastTimer);
		if (time_diff.count() >= 1000) {
			connection->lastTimer = currentTimer;
			connection->sockManager.update(0, 5000);		// 5ms

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

	bool RCConnection::connectServer()
	{
		QSettings settings("settings.ini", QSettings::IniFormat);

		if (getConnected())
			return true;

		printf(":: Initializing %s socket.\n", sock.getDescription());

		// Initialize the socket
		if (sock.init(settings.value("listip","").toString().toStdString().c_str(), settings.value("listport","").toString().toStdString().c_str()) != 0)
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

		//_fileQueue.setCodec(ENCRYPT_GEN_1, 0);
		sendPacket(CString() >> (char)5 >> (char)0 << "GSERV025", true);
		_fileQueue.setCodec(ENCRYPT_GEN_5, key);
		in_codec.setGen(ENCRYPT_GEN_5);
		in_codec.reset(key);

		sendPacket(CString() >> (char)1 >> (char)_account.size() << _account >> (char)_password.size() << _password);

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
		printf(":: Unknown Serverlist Packet: %i (%s)", pPacket.readGUChar(), pPacket.text()+1);
		std::cout <<std::endl;
	}

	void RCConnection::msgSTATUS(CString& pPacket)
	{
		auto status = pPacket.readString("\n");
		std::cout << status.text() << std::endl;
		QMessageBox::information(nullptr, "Serverlist Status", status.text(), QMessageBox::Ok);
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