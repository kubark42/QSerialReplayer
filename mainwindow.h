#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
class MainWindow;
}

class SerialLogging;
class SerialPlayback;
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	QString getFileName(){return fileName;}

private:
	Ui::MainWindow *ui_mainWindow;

	QString fileName;
	bool isConnected;

	SerialLogging *serialLogger;
	SerialPlayback *serialPlayback;
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
	void playbackSpeedChanged(int);

public slots:
	void serialPortConnected();
	void receivedData(QByteArray, quint64 timeStamp);
	void updateTimer(qint64);

private slots:
	// Menu items
	void on_newFileClicked();
	void on_openClicked();
	void on_saveClicked();
	void on_saveAsClicked();
	void actionon_openClicked();
	void actionConfigureSerialDeviceClicked();

	// Slider
	void on_playbackSliderChanged(int);

	// Radio buttons
	void on_baudRateUpdated();
	void on_dataBitsUpdated();
	void on_parityUpdated();
	void on_stopBitsUpdated();

	// Push buttons
	void on_connectClicked();
	void on_clearConsoleClicked();

};

#endif // MAINWINDOW_H
