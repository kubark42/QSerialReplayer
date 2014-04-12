#ifndef SERIALLOGGING_H
#define SERIALLOGGING_H

#include <QtSerialPort/QtSerialPort>
#include <QDateTime>

class QtSerialPort;
class MainWindow;

class SerialLogging : public QObject
{
	Q_OBJECT

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
	QString fileName;
	QFile file;
	QDateTime startTime;

signals:
	void serialPortOpened();
	void newDataReady(QByteArray);
	void newTimeStamp(qint64 ms);

private slots:
	void readData();

	void baudRateUpdated(QSerialPort::BaudRate baudRate);
	void dataBitsUpdated(QSerialPort::DataBits dataBits);
	void parityUpdated(QSerialPort::Parity parity);
	void stopBitsUpdated(QSerialPort::StopBits stopBits);
};

#endif // SERIALLOGGING_H
