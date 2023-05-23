#ifndef TILESEDITOR_RCIODEVICE_H
#define TILESEDITOR_RCIODEVICE_H

#include <QIODevice>
#include "CString.h"

namespace TilesEditor::RC {

	class RCIODevice : public QIODevice {

		Q_OBJECT

		public:
			explicit RCIODevice(QObject *parent, const QString &fileName);

			// Reimplement the following virtual functions:
			qint64 readData(char *data, qint64 maxSize) override;

			qint64 writeData(const char *data, qint64 maxSize) override;

			bool isSequential() const override;

			bool open(OpenMode mode) override;

			void close() override;

		private:
			QString _fileName;

			CString d;
	};
}
#endif //TILESEDITOR_RCIODEVICE_H
