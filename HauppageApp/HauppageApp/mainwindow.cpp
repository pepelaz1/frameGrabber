#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <windows.h>
#include <QFileDialog>
#include <QDateTime>
#include <QtWin>
#include <QThread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_serialPort(0)
{
    ui->setupUi(this);

    m_factory = GraphFactory::getInstance();
    m_capturer = (IRecorderGraph *)m_factory->createNew("Capture");

    populateDevices();
    populatePorts();

    m_settingsFile = QApplication::applicationDirPath() + "/framegrabberapp.ini";
    loadSettings();

    enableControls(false);
    updateSrcControls();

    m_serialPort = new QSerialPort();

    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    m_serialPort->setParity(QSerialPort::EvenParity);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setDataTerminalReady(false);
    m_serialPort->setRequestToSend(false);
   // m_serialPort->setFlowControl(FlowControl)


    connect(m_serialPort, SIGNAL(readyRead()), SLOT(on_readyRead()));
    connect(m_serialPort, SIGNAL(baudRateChanged(qint32, QSerialPort::Directions)),
            SLOT(on_baudRateChanged(qint32, QSerialPort::Directions)));
    connect(m_serialPort, SIGNAL(breakEnabledChanged(bool)), SLOT(on_breakEnabledChanged(bool)));
    connect(m_serialPort, SIGNAL(dataBitsChanged(QSerialPort::DataBits)),
            SLOT(on_dataBitsChanged(QSerialPort::DataBits)));
    connect(m_serialPort, SIGNAL(dataTerminalReadyChanged(bool)),
            SLOT(on_dataTerminalReadyChanged(bool)));
    connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)),
            SLOT(on_error(QSerialPort::SerialPortError)));
    connect(m_serialPort, SIGNAL(flowControlChanged(QSerialPort::FlowControl)),
            SLOT(on_flowControlChanged(QSerialPort::FlowControl)));
    connect(m_serialPort, SIGNAL(parityChanged(QSerialPort::Parity )),
            SLOT(on_parityChanged(QSerialPort::Parity)));
    connect(m_serialPort, SIGNAL(requestToSendChanged(bool)),
            SLOT(on_requestToSendChanged(bool)));
    connect(m_serialPort, SIGNAL(stopBitsChanged(QSerialPort::StopBits)),
            SLOT(on_stopBitsChanged(QSerialPort::StopBits)));


    m_pinouts = QSerialPort::NoSignal;
    initListener();

    on_pb_capture_clicked();

    writeLog("application started");
   // QString s = "Pinout signals: " +  QString::number(m_pinouts);
   // writeLog(s);
}

void MainWindow::initListener()
{
    m_listener = new Listener();
    QThread* thread = new QThread;
    m_listener->moveToThread(thread);

    connect(thread, SIGNAL(started()), m_listener, SLOT(process()));
    connect(m_listener, SIGNAL(quant()), this, SLOT(on_quant()));

    thread->start();
}

void  MainWindow::on_quant()
{
    QSerialPort::PinoutSignals pinouts = m_serialPort->pinoutSignals();
    if (pinouts != m_pinouts)
    {
         QString s = "Pinout signals: " +  QString::number(pinouts);
         writeLog(s);
         m_pinouts = pinouts;

         takePicture();
    }

}

void MainWindow::closeEvent (QCloseEvent *)
{
    m_listener->stop();
    saveSettings();
    writeLog("application closed");
}

MainWindow::~MainWindow()
{

    delete m_listener;
    delete m_serialPort;
    delete ui;
    delete m_capturer;

    GraphFactory::resetInstance();
}

void MainWindow::loadSettings()
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);
    ui->le_output->setText(settings.value("OutputPath", "C:/Output").toString());
    ui->cb_listenport->setCurrentIndex(settings.value("Port", 0).toInt());
}

void MainWindow::saveSettings()
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);
    settings.setValue("OutputPath", ui->le_output->text());
    settings.setValue("Port", ui->cb_listenport->currentIndex());
}

void MainWindow::listen()
{
    if (m_serialPort)
    {
        m_serialPort->close();
        QString serialPortName = ui->cb_listenport->currentText();
        m_serialPort->setPortName(serialPortName);

        if (m_serialPort->open(QIODevice::ReadOnly) == false) {
            QMessageBox::critical(this, "Critical error",
                                  QObject::tr("Failed to open port %1, error: %2").arg(serialPortName).arg(m_serialPort->errorString()));
        }
    }
}

void MainWindow::on_cb_listenport_currentIndexChanged(int)
{
    listen();
}

void MainWindow::writeLog(QString s)
{
     QFile f("framegrabberapp.log");
     f.open(QIODevice::Append | QIODevice::Text);
     QTextStream strm(&f);
     strm << QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss") << "|" << s << "\n";
     f.close();
}

void MainWindow::on_readyRead()
{
     QString data(m_serialPort->readAll());
     writeLog("readyRead signal received, data: " + data
              + ", pinout signals: " +  QString::number(m_serialPort->pinoutSignals()));


     takePicture();
}


void MainWindow::on_baudRateChanged(qint32 baudRate, QSerialPort::Directions directions)
{
    writeLog("baudRateChanged signal received, baudRate: " + QString::number(baudRate)
             + ", directions: " +  QString::number(directions)
             + ", pinout signals: " +  QString::number(m_serialPort->pinoutSignals()));

}
void MainWindow::on_breakEnabledChanged(bool set)
{
    writeLog("breakEnabledChanged signal received, set: " + QString::number(set)
             + ", pinout signals: " +  QString::number(m_serialPort->pinoutSignals()));
}

void MainWindow::on_dataBitsChanged(QSerialPort::DataBits dataBits)
{
    writeLog("dataBitsChanged signal received, dataBits: " + QString::number(dataBits)
             + ", pinout signals: " +  QString::number(m_serialPort->pinoutSignals()));
}

void MainWindow::on_dataTerminalReadyChanged(bool set)
{
    writeLog("dataTerminalReadyChanged signal received, set: " + QString::number(set)
             + ", pinout signals: " +  QString::number(m_serialPort->pinoutSignals()));
}

void MainWindow::on_error(QSerialPort::SerialPortError error)
{
/*  writeLog("error signal received, error: " + QString::number(error)
             + ", pinout signals: " +  QString::number(m_serialPort->pinoutSignals())/); */
}

void MainWindow::on_flowControlChanged(QSerialPort::FlowControl flow)
{
    writeLog("flowControlChanged signal received, flow: " + QString::number(flow)
             + ", pinout signals: " +  QString::number(m_serialPort->pinoutSignals()));
}

void MainWindow::on_parityChanged(QSerialPort::Parity parity)
{
    writeLog("parityChanged signal received, parity: " + QString::number(parity)
             + ", pinout signals: " +  QString::number(m_serialPort->pinoutSignals()));
}

void MainWindow::on_requestToSendChanged(bool set)
{
    writeLog("requestToSendChanged signal received, set: " + QString::number(set)
             + ", pinout signals: " +  QString::number(m_serialPort->pinoutSignals()));
}

void MainWindow::on_stopBitsChanged(QSerialPort::StopBits stopBits)
{
    writeLog("stopBitsChanged signal received, stopBits: " + QString::number(stopBits)
             + ", pinout signals: " +  QString::number(m_serialPort->pinoutSignals()));
}



bool MainWindow::rebuildCap()
{    
    bool result = true;
    setCursor(Qt::WaitCursor);
    m_capturer->close();
    m_capturer->setVideoSource(m_vsrc);

    HRESULT hr = m_capturer->load(m_url, ui->w_vw->winId());
    if (FAILED(hr))
    {
        QMessageBox::warning(this,"error","Failed to build DirectShow graph");
        m_capturer->close();
        result = false;
    }
    setCursor(Qt::ArrowCursor);
    return result;
}

void MainWindow::on_pb_browse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose output path"),  ui->le_output->text(),
                                                    QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (dir != "")
         ui->le_output->setText(dir);
}

void MainWindow::on_pb_capture_clicked()
{
    if (ui->cb_videosrc->currentText() != "")
    {
        if (m_capturer->getState() != IGraph::State_Running)
        {
            m_vsrc = ui->cb_videosrc->currentText();

            if (!rebuildCap())
                return;

            m_capturer->play();
            ui->pb_capture->setText("Stop capture");
            enableControls(true);
        }
        else
        {
             m_capturer->stop();
             ui->pb_capture->setText("Start capture");
             enableControls(false);         
        }
    }
}

void MainWindow::populateDevices()
{
   // Video capture sources
    QStringList lst;
    m_capturer->getVideoSources(lst);
    ui->cb_videosrc->clear();
    foreach (QString s, lst)
        ui->cb_videosrc->addItem(s);
}

void MainWindow::populatePorts()
{
    ui->cb_listenport->addItem("COM1");
    ui->cb_listenport->addItem("COM2");
    ui->cb_listenport->addItem("COM3");
    ui->cb_listenport->addItem("COM4");
}


void MainWindow::updateSrcControls()
{
   ui->cb_videosrc->setEnabled(true);
   ui->cb_listenport->setEnabled(true);
}

void MainWindow::enableControls(bool preview)
{
    bool b = preview ;

    ui->le_output->setDisabled(b);
    ui->pb_browse->setDisabled(b);
    ui->cb_videosrc->setDisabled(b);
   // ui->cb_listenport->setDisabled(b);
    ui->pb_take_picture->setDisabled(!b);
}

void MainWindow::overlayText(IGraph *graph)
{
    graph->clearOverlay();
    graph->addOverlayText(10,10, "Agency: Test agency");
    graph->addOverlayText(10,50, "Room: Test room");
    graph->addOverlayText(10,90, "Camera: Test camera");
    graph->addOverlayText(10,130, QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
    graph->applyOverlay();
}


void MainWindow::on_pb_take_picture_clicked()
{
    takePicture();
}

void MainWindow::takePicture()
{
    if (m_capturer->getState() == IGraph::State_Running)
    {
        HBITMAP bmp  = m_capturer->getLastFrame();
        QPixmap pixmap =  QtWin::fromHBITMAP(bmp);
        QString s = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
        QFile file(ui->le_output->text() + "\\" + s + ".png");
        file.open(QIODevice::WriteOnly);
        pixmap.save(&file, "png");
    }
}
