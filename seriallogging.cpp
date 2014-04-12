#include "mainwindow.h"
#include "seriallogging.h"

SerialLogging::SerialLogging(MainWindow *mainWindow, QSerialPortInfo serialPortDevice, QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits, QSerialPort::Parity parity, QSerialPort::StopBits stopBits)
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

		// Set zero time
		startTime = QDateTime::currentDateTimeUtc();

		// Configure serial port
		serialPort->setBaudRate(baudRate);
		serialPort->setDataBits(dataBits);
		serialPort->setParity(parity);
		serialPort->setStopBits(stopBits);

		connect(this, SIGNAL(serialPortOpened()), mainWindow, SLOT(serialPortConnected()));
		connect(this, SIGNAL(newDataReady(QByteArray)), mainWindow, SLOT(receivedData(QByteArray)));
		connect(this, SIGNAL(newTimeStamp(qint64)), mainWindow, SLOT(updateTimer(qint64)));
		emit serialPortOpened();
	}

	// Connect signals
	connect(serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
	connect(mainWindow, SIGNAL(baudRateChanged(QSerialPort::BaudRate)), this, SLOT(baudRateUpdated(QSerialPort::BaudRate)));
	connect(mainWindow, SIGNAL(dataBitsChanged(QSerialPort::DataBits)), this, SLOT(dataBitsUpdated(QSerialPort::DataBits)));
	connect(mainWindow, SIGNAL(parityChanged(QSerialPort::Parity)), this, SLOT(parityUpdated(QSerialPort::Parity)));
	connect(mainWindow, SIGNAL(stopBitsChanged(QSerialPort::StopBits)), this, SLOT(stopBitsUpdated(QSerialPort::StopBits)));
}

SerialLogging::~SerialLogging()
{
	// Close out the file
	file.close();

	// `delete` closes the serial port if it is still open
	delete serialPort;
}

void SerialLogging::saveToDisk(QByteArray data)
{

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
		file.write(data, sizeof(data));
	}
}

void SerialLogging::baudRateUpdated(QSerialPort::BaudRate baudRate)
{
	serialPort->setBaudRate(baudRate);
}


void SerialLogging::dataBitsUpdated(QSerialPort::DataBits dataBits)
{
	serialPort->setDataBits(dataBits);
}


void SerialLogging::parityUpdated(QSerialPort::Parity parity)
{
	serialPort->setParity(parity);
}


void SerialLogging::stopBitsUpdated(QSerialPort::StopBits stopBits)
{
	serialPort->setStopBits(stopBits);
}


void SerialLogging::readData()
{
	QByteArray data = serialPort->readAll();

	// Save the data to file
	saveToDisk(data);

	// Send the data to any connected listeners
	emit newDataReady(data);

	// Send out the elapsed time to any connected listeners
	emit newTimeStamp(startTime.msecsTo(QDateTime::currentDateTimeUtc()));
}
