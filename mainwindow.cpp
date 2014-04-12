#include <QDebug>
#include <QFile>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "seriallogging.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	mainWindow(new Ui::MainWindow),
	isConnected(false),
	baudRate(QSerialPort::Baud57600),
	dataBits(QSerialPort::Data8),
	parity(QSerialPort::NoParity)
{
	mainWindow->setupUi(this);

	// Populate serial port combo box
	QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
	foreach (QSerialPortInfo serialPort, list) {
		mainWindow->cb_serialPorts->addItem(serialPort.portName());
	}

	// Configure radio buttons
	mainWindow->rb_57600Baud->setChecked(true);
	mainWindow->rb_8DataBits->setChecked(true);
	mainWindow->rb_parityNone->setChecked(true);
	mainWindow->rb_1StopBit->setChecked(true);

	// Connect menu buttons
	connect(mainWindow->actionConfigure_serial_device, SIGNAL(triggered()), this, SLOT(actionConfigureSerialDeviceClicked()));
	connect(mainWindow->actionNew, SIGNAL(triggered()), this, SLOT(newFileClicked()));
	connect(mainWindow->actionOpen, SIGNAL(triggered()), this, SLOT(openClicked()));

	// Connect push buttons
	connect(mainWindow->pb_connectSerialPort, SIGNAL(clicked()), this, SLOT(connectClicked()));

	// Connect radio buttons
	connect(mainWindow->rb_1200Baud, SIGNAL(clicked()), this, SLOT(baudRateUpdated()));
	connect(mainWindow->rb_2400Baud, SIGNAL(clicked()), this, SLOT(baudRateUpdated()));
	connect(mainWindow->rb_4800Baud, SIGNAL(clicked()), this, SLOT(baudRateUpdated()));
	connect(mainWindow->rb_9600Baud, SIGNAL(clicked()), this, SLOT(baudRateUpdated()));
	connect(mainWindow->rb_19200Baud, SIGNAL(clicked()), this, SLOT(baudRateUpdated()));
	connect(mainWindow->rb_38400Baud, SIGNAL(clicked()), this, SLOT(baudRateUpdated()));
	connect(mainWindow->rb_57600Baud, SIGNAL(clicked()), this, SLOT(baudRateUpdated()));
	connect(mainWindow->rb_115200Baud, SIGNAL(clicked()), this, SLOT(baudRateUpdated()));

	connect(mainWindow->rb_5DataBits, SIGNAL(clicked()), this, SLOT(dataBitsUpdated()));
	connect(mainWindow->rb_6DataBits, SIGNAL(clicked()), this, SLOT(dataBitsUpdated()));
	connect(mainWindow->rb_7DataBits, SIGNAL(clicked()), this, SLOT(dataBitsUpdated()));
	connect(mainWindow->rb_8DataBits, SIGNAL(clicked()), this, SLOT(dataBitsUpdated()));

	connect(mainWindow->rb_parityNone, SIGNAL(clicked()), this, SLOT(parityUpdated()));
	connect(mainWindow->rb_parityOdd, SIGNAL(clicked()), this, SLOT(parityUpdated()));
	connect(mainWindow->rb_parityEven, SIGNAL(clicked()), this, SLOT(parityUpdated()));
	connect(mainWindow->rb_parityMark, SIGNAL(clicked()), this, SLOT(parityUpdated()));
	connect(mainWindow->rb_paritySpace, SIGNAL(clicked()), this, SLOT(parityUpdated()));

	connect(mainWindow->rb_1StopBit, SIGNAL(clicked()), this, SLOT(stopBitsUpdated()));
	connect(mainWindow->rb_15StopBit, SIGNAL(clicked()), this, SLOT(stopBitsUpdated()));
	connect(mainWindow->rb_2StopBit, SIGNAL(clicked()), this, SLOT(stopBitsUpdated()));

	// Trigger UI updates
	baudRateUpdated();
	dataBitsUpdated();
	parityUpdated();
	stopBitsUpdated();
}

MainWindow::~MainWindow()
{
	delete mainWindow;
}

void MainWindow::actionOpenClicked()
{

}

void MainWindow::actionConfigureSerialDeviceClicked()
{
//	SerialPortConfigDialog serialPortConfigDialog;

//	if (serialPortConfigDialog.exec() != QDialog::Accepted ) {
//		 return;
//	}
}

void MainWindow::newFileClicked()
{
	fileName = QFileDialog::QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Binary (*.ser)"));
}

void MainWindow::openClicked()
{
	fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"", tr("Binary (*.ser)"));

	// Load file
}

void MainWindow::saveClicked()
{

}

void MainWindow::saveAsClicked()
{
	fileName = QFileDialog::QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Binary (*.ser)"));

	// Call save routine
	saveClicked();
}

void MainWindow::connectClicked()
{
	if (isConnected) {
		mainWindow->pb_connectSerialPort->setText("Connect");
		delete serialLogger;

		// FixMe: Listen for an emitted signal, instead of setting it automatically.
		isConnected=false;
	} else {
		serialPortDevice = QSerialPortInfo(mainWindow->cb_serialPorts->currentText());
		mainWindow->hs_time->setEnabled(false);
		serialLogger = new SerialLogging(this, serialPortDevice, baudRate, dataBits, parity, stopBits);
	}
}

void MainWindow::baudRateUpdated()
{
	if (mainWindow->rb_1200Baud->isChecked()) {
		baudRate = QSerialPort::Baud1200;
	} else if (mainWindow->rb_2400Baud->isChecked()) {
			baudRate = QSerialPort::Baud2400;
	} else if (mainWindow->rb_4800Baud->isChecked()){
		baudRate = QSerialPort::Baud4800;
	} else if (mainWindow->rb_9600Baud->isChecked()){
		baudRate = QSerialPort::Baud9600;
	} else if (mainWindow->rb_19200Baud->isChecked()){
		baudRate = QSerialPort::Baud19200;
	} else if (mainWindow->rb_38400Baud->isChecked()){
		baudRate = QSerialPort::Baud38400;
	} else if (mainWindow->rb_57600Baud->isChecked()){
		baudRate = QSerialPort::Baud57600;
	} else if (mainWindow->rb_115200Baud->isChecked()){
		baudRate = QSerialPort::Baud115200;
	}

	emit baudRateChanged(baudRate);
}

void MainWindow::dataBitsUpdated()
{
	if (mainWindow->rb_5DataBits->isChecked()) {
		dataBits = QSerialPort::Data5;
	} else if (mainWindow->rb_6DataBits->isChecked()){
		dataBits = QSerialPort::Data6;
	} else if (mainWindow->rb_7DataBits->isChecked()){
		dataBits = QSerialPort::Data7;
	} else if (mainWindow->rb_8DataBits->isChecked()){
		dataBits = QSerialPort::Data8;
	}

	emit dataBitsChanged(dataBits);
}

void MainWindow::parityUpdated()
{
	if (mainWindow->rb_parityNone->isChecked()) {
		parity = QSerialPort::NoParity;
	} else if (mainWindow->rb_parityOdd->isChecked()) {
		parity = QSerialPort::OddParity;
	} else if (mainWindow->rb_parityEven->isChecked()) {
		parity = QSerialPort::EvenParity;
	} else if (mainWindow->rb_parityMark->isChecked()) {
		parity = QSerialPort::MarkParity;
	} else if (mainWindow->rb_paritySpace->isChecked()) {
		parity = QSerialPort::SpaceParity;
	}

	emit parityChanged(parity);
}

void MainWindow::stopBitsUpdated()
{
	if (mainWindow->rb_1StopBit->isChecked()) {
		stopBits = QSerialPort::OneStop;
	} else if (mainWindow->rb_15StopBit->isChecked()) {
		stopBits = QSerialPort::OneAndHalfStop;
	} else if (mainWindow->rb_2StopBit->isChecked()) {
		stopBits = QSerialPort::TwoStop;
	}

	emit stopBitsChanged(stopBits);
}

void MainWindow::serialPortConnected()
{
	mainWindow->pb_connectSerialPort->setText("Disconnect");
	isConnected=true;
}

void MainWindow::receivedData(QByteArray data)
{
	mainWindow->tb_serialConsole->append(data);
}


void MainWindow::updateTimer(qint64 elapsedTime_ms)
{
	mainWindow->lcd_seconds->display(floor(elapsedTime_ms/1000));
	mainWindow->lcd_milliseconds->display(floor(elapsedTime_ms % 1000));
}
