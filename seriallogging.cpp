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
#include "seriallogging.h"

SerialLogging::SerialLogging(MainWindow *mainWindow, QSerialPortInfo serialPortDevice, int baudRate, QSerialPort::DataBits dataBits, QSerialPort::Parity parity, QSerialPort::StopBits stopBits)
{
	serialPort = new QSerialPort(serialPortDevice);

	// Open
	int ret = serialPort->open(QIODevice::ReadWrite);

	if (!ret) {
		qDebug() << "Serial port failed to open";
		delete serialPort;
		Q_ASSERT(0);
	} else {
		// Open file
		fileName = mainWindow->getFileName();
		if (fileName != "") {
			file.setFileName(fileName);
			ret = file.open(QIODevice::WriteOnly);
			if (!ret) {
				qDebug() << "File failed to open";
				delete serialPort;
				Q_ASSERT(0);
			}
		}

		// Set start time
		startTime = QDateTime::currentDateTimeUtc();

		// Configure serial port
		serialPort->setBaudRate(baudRate);
		serialPort->setDataBits(dataBits);
		serialPort->setParity(parity);
		serialPort->setStopBits(stopBits);

		connect(this, SIGNAL(serialPortOpened()), mainWindow, SLOT(serialPortConnected()));
		connect(this, SIGNAL(newDataReady(QByteArray, quint64)), mainWindow, SLOT(receivedData(QByteArray, quint64)));
		connect(this, SIGNAL(newTimeStamp(qint64)), mainWindow, SLOT(updateTimer(qint64)));
		emit serialPortOpened();
	}

	// Connect signals
	connect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(mainWindow, SIGNAL(baudRateChanged(int)), this, SLOT(on_baudRateUpdated(int)));
	connect(mainWindow, SIGNAL(dataBitsChanged(QSerialPort::DataBits)), this, SLOT(on_dataBitsUpdated(QSerialPort::DataBits)));
	connect(mainWindow, SIGNAL(parityChanged(QSerialPort::Parity)), this, SLOT(on_parityUpdated(QSerialPort::Parity)));
	connect(mainWindow, SIGNAL(stopBitsChanged(QSerialPort::StopBits)), this, SLOT(on_stopBitsUpdated(QSerialPort::StopBits)));
}

SerialLogging::~SerialLogging()
{
	// Close out the file
	file.close();

	// 'delete' closes the serial port if it is still open
	delete serialPort;
}

void SerialLogging::saveToDisk(QByteArray data)
{
	// ToDo: Handle the case where lenth(data) > 255

	quint32 ellapsedTicks = startTime.msecsTo(QDateTime::currentDateTimeUtc())/10;

	QByteArray packet;
	packet.append(SYNC0);
	packet.append(SYNC1);
	packet.append(reinterpret_cast<const char*>(&ellapsedTicks), sizeof(ellapsedTicks));
	packet.append(data.size());
	packet.append(data);
	packet.append(END_MARKER);

	// If the file is open, write the data
	if (file.isOpen()) {
		file.write(packet, packet.size());
	}
}

void SerialLogging::on_baudRateUpdated(int baudRate)
{
	serialPort->setBaudRate(baudRate);
}


void SerialLogging::on_dataBitsUpdated(QSerialPort::DataBits dataBits)
{
	serialPort->setDataBits(dataBits);
}


void SerialLogging::on_parityUpdated(QSerialPort::Parity parity)
{
	serialPort->setParity(parity);
}


void SerialLogging::on_stopBitsUpdated(QSerialPort::StopBits stopBits)
{
	serialPort->setStopBits(stopBits);
}


void SerialLogging::readData()
{
	// Get timestamp immediately
	quint64 timeStamp = startTime.msecsTo(QDateTime::currentDateTimeUtc());

	// Read data from serial port
	QByteArray data = serialPort->readAll();

	// Save the data to file
	saveToDisk(data);

	// Send the data to any connected listeners
	emit newDataReady(data, timeStamp);

	// Send out the elapsed time to any connected listeners
	emit newTimeStamp(timeStamp);
}
