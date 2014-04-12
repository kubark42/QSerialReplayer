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


#ifndef SERIALLOGGING_H
#define SERIALLOGGING_H

#include <QtSerialPort/QtSerialPort>

class MainWindow;

class SerialLogging : public QObject
{
	Q_OBJECT

	// FixMe: This shouldn't be repeated both here and in seriallogging.h
	enum {
		SYNC0 = 0x53,
		SYNC1 = 0x01,
		END_MARKER = 0x0A
	};

public:
	SerialLogging(MainWindow *mainWindow, QSerialPortInfo serialPortDevice, QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits, QSerialPort::Parity parity, QSerialPort::StopBits stopBits);
	~SerialLogging();

private:
	void saveToDisk(QByteArray data);

	QSerialPort *serialPort;
	QFile file;
	QString fileName;
	QDateTime startTime;

signals:
	void serialPortOpened();
	void newDataReady(QByteArray);
	void newTimeStamp(qint64 ms);

private slots:
	void readData();

	void on_baudRateUpdated(QSerialPort::BaudRate baudRate);
	void on_dataBitsUpdated(QSerialPort::DataBits dataBits);
	void on_parityUpdated(QSerialPort::Parity parity);
	void on_stopBitsUpdated(QSerialPort::StopBits stopBits);
};

#endif // SERIALLOGGING_H
