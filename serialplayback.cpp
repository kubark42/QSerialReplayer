/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */


#include "mainwindow.h"
#include "serialplayback.h"

SerialPlayback::SerialPlayback(MainWindow *mainWindow) :
	logPlaybackPeriod(10),
	fsmStatus(FSM_SYNC0)
{
	fileName = mainWindow->getFileName();
	fileName = "/Users/kenz/Desktop/1212.ser";

	if (fileName != "") {
		loadFile(fileName);

		if (file.isOpen()) {
			// Set start time
			logTime = 0;

			logHeartbeat.start(logPlaybackPeriod);

			// Connect signals
			connect(&logHeartbeat, SIGNAL(timeout()), this, SLOT(on_timerTimeout()));
			connect(mainWindow, SIGNAL(playbackSpeedChanged(int)), this, SLOT(on_playbackSpeedChanged(int)));
			connect(this, SIGNAL(newDataReady(QByteArray, quint64)), mainWindow, SLOT(receivedData(QByteArray, quint64)));
		}
	}
}

SerialPlayback::~SerialPlayback()
{
	file.close();
}


void SerialPlayback::loadFile(QString fileName)
{
	// Open file
	if (fileName != "") {
		file.setFileName(fileName);
		int ret = file.open(QIODevice::ReadOnly);
		if (!ret) {
			qDebug() << "File failed to open";
			Q_ASSERT(0);
		}
	}
}

void SerialPlayback::parseByte(quint8 c)
{
	switch (fsmStatus) {
	case FSM_SYNC0:
		if (c == SYNC0) {
			fsmStatus = FSM_SYNC1;
		} else {
			fsmStatus = FSM_LOST_SYNC;
		}

		break;
	case FSM_SYNC1:
		if (c == SYNC1) {
			fsmStatus = FSM_TIMESTAMP;
			idx = 0;
			timeStamp=0;
		} else {
			fsmStatus = FSM_LOST_SYNC;
		}

		break;
	case FSM_TIMESTAMP:
		//
		timeStamp += (c) * (1<< 8*idx);
		idx++;

		if (idx == 4) {
			fsmStatus = FSM_DATALENGTH;
		}

		break;
	case FSM_DATALENGTH:
		dataLength = c;
		fsmStatus = FSM_DATA;
		idx = 0;
		break;
	case FSM_DATA:
		data.append(c);
		idx++;

		if (idx == dataLength) {
			fsmStatus = FSM_END_MARKER;
		}
		break;
	case FSM_END_MARKER:
		if (c == END_MARKER) {
			fsmStatus = FSM_VALID_MSG;
		} else {
			fsmStatus = FSM_LOST_SYNC;
		}
		break;
	case FSM_VALID_MSG:
		break;
	case FSM_LOST_SYNC:
		fsmStatus = FSM_SYNC0;
		break;
	}
}

void SerialPlayback::on_timerTimeout()
{
	logTime += 1;

	while (fsmStatus != FSM_VALID_MSG && !file.atEnd()) {
		QByteArray fileData;

		// Try to read the packet intelligently. This is less
		// efficient than reading lots of data at once, but
		// easier to program.
		if (fsmStatus != FSM_DATA) {
			fileData = file.read(1);
		}
		else {
			fileData = file.read(dataLength);
		}
		foreach (quint8 byte, fileData) {
			parseByte(byte);
		}
	}

	// Check if the packet needs to be emitted
	if (logTime > timeStamp && fsmStatus == FSM_VALID_MSG) {
		emit newDataReady(data, timeStamp);

		data.clear();
		fsmStatus = FSM_SYNC0;
	}

	// If the file has been read, end playback
	if (file.atEnd()) {
		emit playbackCompleted();
	}
}


void SerialPlayback::on_playbackSpeedChanged(int newPeriod)
{
	logPlaybackPeriod = newPeriod;
	logHeartbeat.start(logPlaybackPeriod);
}
