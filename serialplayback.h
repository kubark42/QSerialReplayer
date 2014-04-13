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


#ifndef SERIALPLAYBACK_H
#define SERIALPLAYBACK_H

#include <QtSerialPort/QtSerialPort>

class MainWindow;

class SerialPlayback : public QObject
{
	Q_OBJECT

	enum FSM_STATUS {
		FSM_SYNC0,
		FSM_SYNC1,
		FSM_TIMESTAMP,
		FSM_DATALENGTH,
		FSM_DATA,
		FSM_END_MARKER,
		FSM_VALID_MSG,
		FSM_LOST_SYNC
	};

	// FixMe: This shouldn't be repeated both here and in seriallogging.h
	enum {
		SYNC0 = 0x53,
		SYNC1 = 0x01,
		END_MARKER = 0x0A
	};

public:
	SerialPlayback(MainWindow *mainWindow);
	~SerialPlayback();

private:
	void parseByte(quint8 c);
	void loadFile(QString fileName);

	quint32 timeStamp;
	quint8 dataLength;
	QByteArray data;
	quint8 idx;

	QFile file;
	QString fileName;
	quint32 logTime;
	QTimer logHeartbeat;
	qint16 logPlaybackPeriod;

	FSM_STATUS fsmStatus;

signals:
	void playbackCompleted();
	void newDataReady(QByteArray, quint64);

private slots:
	void on_playbackSpeedChanged(int);
	void on_timerTimeout();
};

#endif // SERIALPLAYBACK_H
