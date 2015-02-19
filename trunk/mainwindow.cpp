/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"

#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <fdevice.h>
#include "util.h"

//using namespace firmatator;

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//! [0]
    ui->setupUi(this);
    console = new Console;
    console->setEnabled(false);
    ui->gridLayout->addWidget(console);
//    setCentralWidget(console);
//! [1]
    serial = new QSerialPort(this);
//! [1]
    settings = new SettingsDialog;

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    initActionsConnections();

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(SetPinArduino()));
    connect(ui->DigWrite,SIGNAL(clicked()),this,SLOT(DWrite()));
    connect(ui->DigWrite_2 ,SIGNAL(clicked()),this,SLOT(DWrite12()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(DevOpen()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(DevClose()));
    connect(ui->pushButton_4,SIGNAL(clicked()),this,SLOT(SetPin12()));
    connect(ui->stepButton,SIGNAL(clicked()),this,SLOT(step()));


//! [2]
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
//! [2]
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
//! [3]
//!
    ix=0;
    for (int i=0; i<10; i++){
//        MaindigitalInputData[i]=0x00;
        MaindigitalOutputData[i]=0x00;
    }
    this->arduino = new FDevice("/dev/ttyUSB0");
    connect(this->arduino, SIGNAL(deviceReady()), this, SLOT(start()));

    qDebug("Connecting...");

    if (arduino->connect())
    {
        qDebug("Succesfully connected");
    }
    else
    {
        qDebug("Error connecting");
    }

}
//! [3]

MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

//! [4]
void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
            console->setEnabled(true);
            console->setLocalEchoEnabled(p.localEchoEnabled);
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
            ui->actionConfigure->setEnabled(false);
            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        ui->statusBar->showMessage(tr("Open error"));
    }
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    serial->close();
    console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}
//! [6]

//! [7]
void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    console->putData(data);
}
//! [7]

//! [8]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));
    connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}
//! [9]
void MainWindow::start()
{
    activeTimer = new QTimer(this);
    activeTimer->setInterval(1*1000);
    activeTimer->setSingleShot(false);
    connect(activeTimer, SIGNAL(timeout()), this, SLOT(step()));

    qDebug() << "Initializing ports...";

    arduino->pinMode(13, FDevice::PINMODE_OUTPUT);
    //arduino->pinMode(3, FDevice::PINMODE_OUTPUT);
    arduino->pinMode(3, FDevice::PINMODE_PWM);
    arduino->pinMode(2, FDevice::PINMODE_SERVO);

    arduino->pinMode(8, FDevice::PINMODE_INPUT);
    arduino->pinMode(9, FDevice::PINMODE_INPUT);
    arduino->pinMode(14, FDevice::PINMODE_ANALOG);

    arduino->pinMode(18, FDevice::PINMODE_I2C);
    arduino->pinMode(19, FDevice::PINMODE_I2C);

    int readcmd[] = {0x00};
    int readbyte[] = {0x02};
    arduino->I2CConfig(0, 100);
    arduino->I2CRequest(0x70, readcmd, FDevice::I2C_MODE_READ_ONCE);
    //arduino->I2CRequest(0x70, readcmd, FDevice::I2C_MODE_READ_ONCE);

    qDebug() << "Succesfully initialized.";

    qDebug() << "Starting loop...";
//    activeTimer->start();
}
//! [10]
void MainWindow::step()
{
    if (arduino->available())
    {
        ix++;

        qDebug() << "";
        qDebug() << "----------------------------------------------------------";
        qDebug() << "";
        qDebug() << "Iteraction " << ix;// << dec << i;

        if (ix % 2 == 0)
        {
            d = FDevice::DIGITAL_HIGH;
        }
        else
        {
            d = FDevice::DIGITAL_LOW;
        }

        arduino->digitalWrite(13, d);
        //arduino->digitalWrite(3, d);

        if (a >= 250)
            a_ud = 1;
        else if (a <= 0)
            a_ud = 0;

        if (a_ud == 0)
            a += 50;
        else
            a -= 50;

        if (s >= 180)
            s_ud = 1;
        else if (s <= 0)
            s_ud = 0;

        if (s_ud == 0)
            s += 20;
        else
            s -= 20;


        arduino->analogWrite(3, a);
        arduino->setServo(2, s);


        qDebug() << "Digital Out (3):" << d << " / Analog out (3): " << a << " / Servo out (2): " << s;


        QTimer* wait = new QTimer(this);
        wait->setInterval(100);
        wait->setSingleShot(true);
        connect(wait, SIGNAL(timeout()), this, SLOT(printInputs()));
        wait->start();
    }
    else
    {
        qDebug() << "Connection with device lost or device not ready...";
    }
}
//! [11]
void MainWindow::printInputs()
{
    qDebug() << "Digital In (8): " << arduino->digitalRead(8) << " / Analog In (14, 15, 16, 17, 18, 19): " <<  arduino->analogRead(14) << ", " << arduino->analogRead(15) << ", " << arduino->analogRead(16) << ", " << arduino->analogRead(17) << ", " << arduino->analogRead(18) << ", " << arduino->analogRead(19);
}

//! [12]
void MainWindow::SetPinArduino()
{
//    if (arduino->available())
//    {

//        serial->setBaudRate(QSerialPort::Baud57600);
//        serial->open(QIODevice::ReadWrite) ;
//        char* buffer = (char*) malloc(3);
//        buffer[0] = 0x90 | 1;
//        buffer[1] = 0xFF & 0x7F;
//        buffer[2] = 0xFF & 0x7F;
//        //QByteArray s = QByteArray::fromRawData(buffer, 3);
//        QByteArray s = QByteArray(buffer, 3);
//        //qDebug() << s;

//        serial->write(s);
//        //qDebug() << serial->bytesToWrite();
//        serial->flush();

    if (d == FDevice::DIGITAL_LOW)
    {
        d = FDevice::DIGITAL_HIGH;
    }
    else
    {
        d = FDevice::DIGITAL_LOW;
    }
    int value = d;
    int pin = 13;
    int portNumber = (pin >> 3) & 0x0F;

    if (value == 0)
         MaindigitalOutputData[portNumber] &= ~(1 << (pin & 0x07));
    else
         MaindigitalOutputData[portNumber] |= (1 << (pin & 0x07));

    char* buffer = (char*) malloc(3);
    buffer[0] = arduino->COMMAND_DIGITAL_MESSAGE | portNumber;
    buffer[1] = MaindigitalOutputData[portNumber] & 0x7F;
    buffer[2] = (MaindigitalOutputData[portNumber] >> 7) & 0x7F;
    //QByteArray s = QByteArray::fromRawData(buffer, 3);
    QByteArray s = QByteArray(buffer, 3);
    qDebug() << s;

    serial->write(s);
    //qDebug() << serial->bytesToWrite();
    serial->flush();

////    d = FDevice::DIGITAL_HIGH;
//    arduino->digitalWrite(13, d);
    qDebug() << "Digital Port Number (Internal array):" <<  MaindigitalOutputData[portNumber] << "port# " <<portNumber ;
    qDebug() << "Command: " << lowerToHex(arduino->COMMAND_DIGITAL_MESSAGE | portNumber) << " / LSB: " << lowerToHex(MaindigitalOutputData[portNumber] & 0x7F) << " / MSB: " << lowerToHex(MaindigitalOutputData[portNumber] >> 7);
    qDebug() << "Digital Out (13):" << d << " / Analog out (3): " << a << " / Servo out (2): " << s;
//  }
//    qDebug() << "Digital In (8): " << arduino->digitalRead(8) << " / Analog In (14, 15, 16, 17, 18, 19): " <<  arduino->analogRead(14) << ", " << arduino->analogRead(15) << ", " << arduino->analogRead(16) << ", " << arduino->analogRead(17) << ", " << arduino->analogRead(18) << ", " << arduino->analogRead(19);
}

//! [13]
void MainWindow::DWrite()
{
// if (arduino->available())
//  {
    if (d == FDevice::DIGITAL_LOW)
    {
        d = FDevice::DIGITAL_HIGH;
    }
    else
    {
        d = FDevice::DIGITAL_LOW;
    }
    QByteArray s=arduino->digitalWrite(13, d);
    writeData(s);
    qDebug() << "Digital Out (13):" << d << " / Analog out (3): " << a << " / Servo out (2): " << s;
//  }

}

void MainWindow::DWrite12()
{
// if (arduino->available())
//  {
    if (d == FDevice::DIGITAL_LOW)
    {
        d = FDevice::DIGITAL_HIGH;
    }
    else
    {
        d = FDevice::DIGITAL_LOW;
    }
    QByteArray s=arduino->digitalWrite(12, d);
    writeData(s);
    qDebug() << "Digital Out (12):" << d << " / Analog out (3): " << a << " / Servo out (2): " << s;
//  }

}

void MainWindow::DevClose()
{
//    activeTimer->stop();
    arduino->disconnect();

}
void MainWindow::DevOpen()
{
//    activeTimer->start();
    arduino->connect();

}

void MainWindow::digitalWrite(int pin, int value)
{
    qDebug() << "DigitalWrite : pin " <<  pin << " val " << value;

    int portNumber = (pin >> 3) & 0x0F;

    if (value == 0)
         MaindigitalOutputData[portNumber] &= ~(1 << (pin & 0x07));
    else
         MaindigitalOutputData[portNumber] |= (1 << (pin & 0x07));

    /*
    char* buffer = (char*) malloc(6);
    //char buffer[2];

    buffer[1] = lowerToHex(COMMAND_DIGITAL_MESSAGE | portNumber);
    buffer[0] = upperToHex(COMMAND_DIGITAL_MESSAGE | portNumber);
    buffer[3] = lowerToHex(digitalOutputData[portNumber] & 0x7F);
    buffer[2] = upperToHex(digitalOutputData[portNumber] & 0x7F);
    buffer[5] = lowerToHex(digitalOutputData[portNumber] >> 7);
    buffer[4] = upperToHex(digitalOutputData[portNumber] >> 7);

    //QByteArray s = QByteArray(buffer, 6);
    QByteArray s = QByteArray::fromRawData(buffer, 6);
    qDebug() << s;
    */

    char* buffer = (char*) malloc(3);
    buffer[0] = arduino->COMMAND_DIGITAL_MESSAGE | portNumber;
    buffer[1] = MaindigitalOutputData[portNumber] & 0x7F;
    buffer[2] = (MaindigitalOutputData[portNumber] >> 7) & 0x7F;
//    QByteArray s = QByteArray::fromRawData(buffer, 3);
    QByteArray s = QByteArray(buffer, 3);
    qDebug() << s;

    serial->write(s);
    qDebug() << serial->bytesToWrite();
    serial->flush();


    qDebug() << "Digital Port Number (Internal array):" <<  MaindigitalOutputData[portNumber] << "port# " <<portNumber ;
    qDebug() << "Command: " << lowerToHex(arduino->COMMAND_DIGITAL_MESSAGE | portNumber) << " / LSB: " << lowerToHex(MaindigitalOutputData[portNumber] & 0x7F) << " / MSB: " << lowerToHex(MaindigitalOutputData[portNumber] >> 7);
}

void MainWindow::SetPin12()
{
//    if (arduino->available())
//    {
        if (d12 == FDevice::DIGITAL_LOW)
        {
            d12 = FDevice::DIGITAL_HIGH;
        }
        else
        {
            d12 = FDevice::DIGITAL_LOW;
        }

      digitalWrite(12,d12);
//    }

}
