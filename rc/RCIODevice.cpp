#include <QTextStream>
#include "RCIODevice.h"
#include "RCConnection.h"

namespace TilesEditor::RC {

	RCIODevice::RCIODevice(QObject *parent, const QString &fileName) : QIODevice(parent) {
		_fileName = fileName;
	}

	qint64 RCIODevice::writeData(const char *data, qint64 maxSize) {
		return 0;
	}

	qint64 RCIODevice::readData(char *data, qint64 maxSize) {
		auto connection = RCConnection::getInstance();
		if (d.isEmpty())
			d = connection->getFiles()[_fileName.toStdString()];



		if (d.bytesLeft() > 0) {
			qint64 bytesToRead = qMin(maxSize, d.length());  // Calculate the number of bytes to read
			auto d1 = d.readChars(bytesToRead).text();
			memcpy(data, d1, bytesToRead);  // Copy the data to the 'data' buffer
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