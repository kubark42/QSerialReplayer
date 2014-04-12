#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
class MainWindow;
}

class SerialLogging;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	QString getFileName(){return fileName;}

private:
	Ui::MainWindow *mainWindow;

	QString fileName;
	bool isConnected;

	SerialLogging *serialLogger;
	QSerialPortInfo serialPortDevice;
	QSerialPort::BaudRate baudRate;
	QSerialPort::DataBits dataBits;
	QSerialPort::Parity parity;
	QSerialPort::StopBits stopBits;

signals:
	void baudRateChanged(QSerialPort::BaudRate);
	void dataBitsChanged(QSerialPort::DataBits);
	void parityChanged(QSerialPort::Parity);
	void stopBitsChanged(QSerialPort::StopBits);

public slots:
	void serialPortConnected();
	void receivedData(QByteArray);
	void updateTimer(qint64);

private slots:
	// Menu items
	void newFileClicked();
	void openClicked();
	void saveClicked();
	void saveAsClicked();
	void actionOpenClicked();
	void actionConfigureSerialDeviceClicked();

	// Radio buttons
	void baudRateUpdated();
	void dataBitsUpdated();
	void parityUpdated();
	void stopBitsUpdated();

	void connectClicked();

};

#endif // MAINWINDOW_H
