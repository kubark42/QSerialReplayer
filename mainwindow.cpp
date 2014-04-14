#include <QDebug>
#include <QFile>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "seriallogging.h"
#include "serialplayback.h"
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui_mainWindow(new Ui::MainWindow),
	isConnected(false),
	baudRate(QSerialPort::Baud57600),
	dataBits(QSerialPort::Data8),
	parity(QSerialPort::NoParity)
{
	// Inspired from http://stackoverflow.com/questions/14960472/running-c-binary-from-inside-qt-and-redirecting-the-output-of-the-binary-to-a
	// and http://justcheckingonall.wordpress.com/2009/06/09/howto-vsp-socat/
	QProcess process;
				process.start(":/external_binaries/socat.osx",
								  QStringList() << "-d" << "-d" << "pty,raw,echo=0"  << "pty,raw,echo=0",
								  QIODevice::ReadWrite);

	if(!process.waitForFinished()) // beware the timeout default parameter
		 qDebug() << "executing program failed with exit code" << process.exitCode();
	else
		 qDebug() << QString(process.readAllStandardOutput()).split('\n');


	ui_mainWindow->setupUi(this);

	// Populate serial port combo box
	QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
	foreach (QSerialPortInfo serialPort, list) {
		ui_mainWindow->cb_serialPorts->addItem(serialPort.portName());
	}

	// Configure radio buttons
	ui_mainWindow->rb_57600Baud->setChecked(true);
	ui_mainWindow->rb_8DataBits->setChecked(true);
	ui_mainWindow->rb_parityNone->setChecked(true);
	ui_mainWindow->rb_1StopBit->setChecked(true);

	// Connect menu buttons
	connect(ui_mainWindow->actionConfigure_serial_device, SIGNAL(triggered()), this, SLOT(actionConfigureSerialDeviceClicked()));
	connect(ui_mainWindow->actionNew, SIGNAL(triggered()), this, SLOT(on_newFileClicked()));
	connect(ui_mainWindow->actionOpen, SIGNAL(triggered()), this, SLOT(on_openClicked()));

	// Connect push buttons
	connect(ui_mainWindow->pb_connectSerialPort, SIGNAL(clicked()), this, SLOT(on_connectClicked()));
	connect(ui_mainWindow->pb_clearConsole, SIGNAL(clicked()), this, SLOT(on_clearConsoleClicked()));

	// Connect slider
	connect(ui_mainWindow->hs_time, SIGNAL(valueChanged(int)), this, SLOT(on_playbackSliderChanged(int)));

	// Connect radio buttons
	connect(ui_mainWindow->rb_1200Baud, SIGNAL(clicked()), this, SLOT(on_baudRateUpdated()));
	connect(ui_mainWindow->rb_2400Baud, SIGNAL(clicked()), this, SLOT(on_baudRateUpdated()));
	connect(ui_mainWindow->rb_4800Baud, SIGNAL(clicked()), this, SLOT(on_baudRateUpdated()));
	connect(ui_mainWindow->rb_9600Baud, SIGNAL(clicked()), this, SLOT(on_baudRateUpdated()));
	connect(ui_mainWindow->rb_19200Baud, SIGNAL(clicked()), this, SLOT(on_baudRateUpdated()));
	connect(ui_mainWindow->rb_38400Baud, SIGNAL(clicked()), this, SLOT(on_baudRateUpdated()));
	connect(ui_mainWindow->rb_57600Baud, SIGNAL(clicked()), this, SLOT(on_baudRateUpdated()));
	connect(ui_mainWindow->rb_115200Baud, SIGNAL(clicked()), this, SLOT(on_baudRateUpdated()));

	connect(ui_mainWindow->rb_5DataBits, SIGNAL(clicked()), this, SLOT(on_dataBitsUpdated()));
	connect(ui_mainWindow->rb_6DataBits, SIGNAL(clicked()), this, SLOT(on_dataBitsUpdated()));
	connect(ui_mainWindow->rb_7DataBits, SIGNAL(clicked()), this, SLOT(on_dataBitsUpdated()));
	connect(ui_mainWindow->rb_8DataBits, SIGNAL(clicked()), this, SLOT(on_dataBitsUpdated()));

	connect(ui_mainWindow->rb_parityNone, SIGNAL(clicked()), this, SLOT(on_parityUpdated()));
	connect(ui_mainWindow->rb_parityOdd, SIGNAL(clicked()), this, SLOT(on_parityUpdated()));
	connect(ui_mainWindow->rb_parityEven, SIGNAL(clicked()), this, SLOT(on_parityUpdated()));
	connect(ui_mainWindow->rb_parityMark, SIGNAL(clicked()), this, SLOT(on_parityUpdated()));
	connect(ui_mainWindow->rb_paritySpace, SIGNAL(clicked()), this, SLOT(on_parityUpdated()));

	connect(ui_mainWindow->rb_1StopBit, SIGNAL(clicked()), this, SLOT(on_stopBitsUpdated()));
	connect(ui_mainWindow->rb_15StopBit, SIGNAL(clicked()), this, SLOT(on_stopBitsUpdated()));
	connect(ui_mainWindow->rb_2StopBit, SIGNAL(clicked()), this, SLOT(on_stopBitsUpdated()));

	// Trigger UI updates
	on_baudRateUpdated();
	on_dataBitsUpdated();
	on_parityUpdated();
	on_stopBitsUpdated();
}

MainWindow::~MainWindow()
{
	delete ui_mainWindow;
}

void MainWindow::actionon_openClicked()
{

}

void MainWindow::actionConfigureSerialDeviceClicked()
{
//	SerialPortConfigDialog serialPortConfigDialog;

//	if (serialPortConfigDialog.exec() != QDialog::Accepted ) {
//		 return;
//	}
}

void MainWindow::on_newFileClicked()
{
	fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Binary (*.ser)"));
}

void MainWindow::on_openClicked()
{
	fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"", tr("Binary (*.ser)"));

	// Load file
	serialPlayback = new SerialPlayback(this);

	//TODO: Delete serialPlayback
}

void MainWindow::on_saveClicked()
{

}

void MainWindow::on_saveAsClicked()
{
	fileName = QFileDialog::QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Binary (*.ser)"));

	// Call save routine
	on_saveClicked();
}

void MainWindow::on_connectClicked()
{
	if (isConnected) {
		ui_mainWindow->pb_connectSerialPort->setText("Connect");
		if (serialLogger != NULL) {
			delete serialLogger;
		}

		// FixMe: Listen for an emitted signal, instead of setting it automatically.
		isConnected=false;
	} else {
		serialPortDevice = QSerialPortInfo(ui_mainWindow->cb_serialPorts->currentText());
		ui_mainWindow->hs_time->setEnabled(false);
		serialLogger = new SerialLogging(this, serialPortDevice, baudRate, dataBits, parity, stopBits);
	}
}

void MainWindow::on_baudRateUpdated()
{
	if (ui_mainWindow->rb_1200Baud->isChecked()) {
		baudRate = QSerialPort::Baud1200;
	} else if (ui_mainWindow->rb_2400Baud->isChecked()) {
			baudRate = QSerialPort::Baud2400;
	} else if (ui_mainWindow->rb_4800Baud->isChecked()){
		baudRate = QSerialPort::Baud4800;
	} else if (ui_mainWindow->rb_9600Baud->isChecked()){
		baudRate = QSerialPort::Baud9600;
	} else if (ui_mainWindow->rb_19200Baud->isChecked()){
		baudRate = QSerialPort::Baud19200;
	} else if (ui_mainWindow->rb_38400Baud->isChecked()){
		baudRate = QSerialPort::Baud38400;
	} else if (ui_mainWindow->rb_57600Baud->isChecked()){
		baudRate = QSerialPort::Baud57600;
	} else if (ui_mainWindow->rb_115200Baud->isChecked()){
		baudRate = QSerialPort::Baud115200;
	}

	emit baudRateChanged(baudRate);
}

void MainWindow::on_dataBitsUpdated()
{
	if (ui_mainWindow->rb_5DataBits->isChecked()) {
		dataBits = QSerialPort::Data5;
	} else if (ui_mainWindow->rb_6DataBits->isChecked()){
		dataBits = QSerialPort::Data6;
	} else if (ui_mainWindow->rb_7DataBits->isChecked()){
		dataBits = QSerialPort::Data7;
	} else if (ui_mainWindow->rb_8DataBits->isChecked()){
		dataBits = QSerialPort::Data8;
	}

	emit dataBitsChanged(dataBits);
}

void MainWindow::on_parityUpdated()
{
	if (ui_mainWindow->rb_parityNone->isChecked()) {
		parity = QSerialPort::NoParity;
	} else if (ui_mainWindow->rb_parityOdd->isChecked()) {
		parity = QSerialPort::OddParity;
	} else if (ui_mainWindow->rb_parityEven->isChecked()) {
		parity = QSerialPort::EvenParity;
	} else if (ui_mainWindow->rb_parityMark->isChecked()) {
		parity = QSerialPort::MarkParity;
	} else if (ui_mainWindow->rb_paritySpace->isChecked()) {
		parity = QSerialPort::SpaceParity;
	}

	emit parityChanged(parity);
}

void MainWindow::on_stopBitsUpdated()
{
	if (ui_mainWindow->rb_1StopBit->isChecked()) {
		stopBits = QSerialPort::OneStop;
	} else if (ui_mainWindow->rb_15StopBit->isChecked()) {
		stopBits = QSerialPort::OneAndHalfStop;
	} else if (ui_mainWindow->rb_2StopBit->isChecked()) {
		stopBits = QSerialPort::TwoStop;
	}

	emit stopBitsChanged(stopBits);
}

void MainWindow::serialPortConnected()
{
	ui_mainWindow->pb_connectSerialPort->setText("Disconnect");
	isConnected=true;
}

void MainWindow::receivedData(QByteArray data, quint64 timeStamp)
{
	QTextCharFormat fmt;
	fmt.setBackground(QColor(255-(rand()%30), 255-(rand()%30), 255-(rand()%30)));

	{
		QTextCursor cursor = ui_mainWindow->pte_serialConsole->textCursor();

		// Set background color
		cursor.setCharFormat(fmt);

		// Choose Cursor.insertText() approach over "TextEdit.appendPlainText()" because
		// it's orders of magnitude faster. See https://bugreports.qt-project.org/browse/QTBUG-3554
		cursor.beginEditBlock();

		if (ui_mainWindow->rb_displayASCII->isChecked()) {
			cursor.insertText(data);
		} else if (ui_mainWindow->rb_displayHex->isChecked()) {
			for (int i=0; i<data.size(); i++) {
				QByteArray tmpData;
				tmpData.append(data[i]);

				cursor.insertText("0x");
				cursor.insertText(tmpData.toHex());
				cursor.insertText(" ");
			}
		}
		cursor.endEditBlock();
	}

	{
		QTextCursor cursor = ui_mainWindow->pte_sampleTimes->textCursor();

		// Set background color
		cursor.setCharFormat(fmt);

		// Choose Cursor.insertText() approach over "TextEdit.appendPlainText()" because
		// it's orders of magnitude faster. See https://bugreports.qt-project.org/browse/QTBUG-3554
		cursor.beginEditBlock();
		cursor.insertText(QString::number(timeStamp/100.0) + QString("\n"));
		cursor.endEditBlock();
	}
}


void MainWindow::updateTimer(qint64 elapsedTime_ms)
{
	ui_mainWindow->lcd_seconds->display(floor(elapsedTime_ms/1000));
	ui_mainWindow->lcd_milliseconds->display(floor(elapsedTime_ms % 1000));
}


void MainWindow::on_playbackSliderChanged(int newPeriod)
{
	emit playbackSpeedChanged(newPeriod);
}


void MainWindow::on_clearConsoleClicked()
{
	ui_mainWindow->pte_serialConsole->clear();
	ui_mainWindow->pte_sampleTimes->clear();
}
