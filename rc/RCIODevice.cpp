#include <QTextStream>
#include "RCIODevice.h"
#include "RCConnection.h"

namespace TilesEditor::RC {

	RCIODevice::RCIODevice(QObject *parent, const QString &fileName) : QIODevice(parent) {
		_fileName = fileName.toStdString();
		size_t lastSlashPos = _fileName.find_last_of('/');

		if (lastSlashPos != std::string::npos) {
			// Extracting the substring up to the last '/'
			_fileName = _fileName.substr(lastSlashPos + 1);
		}
	}

	qint64 RCIODevice::writeData(const char *data, qint64 maxSize) {
		return 0;
	}

	qint64 RCIODevice::readData(char *data, qint64 maxSize) {
		auto connection = RCConnection::getInstance();
		if (d.isEmpty()) {
			d << connection->getFiles()[_fileName];
			d.setRead(0);
		}

		if (d.bytesLeft() > 0) {
			qint64 bytesToRead = qMin(maxSize, d.length());  // Calculate the number of bytes to read
			auto d1 = d.readChars(bytesToRead);
			auto d2 = d1.text();
			memcpy(data, d2, bytesToRead);  // Copy the data to the 'data' buffer
			return bytesToRead;
		}

		return 0;
	}

	bool RCIODevice::isSequential() const {
		return QIODevice::isSequential();
	}

	bool RCIODevice::open(QIODeviceBase::OpenMode mode) {
		return QIODevice::open(mode);
	}

	void RCIODevice::close() {
		QIODevice::close();
	}

}