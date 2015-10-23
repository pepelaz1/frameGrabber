#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <windows.h>
#include <QFileDialog>
#include <QDateTime>
#include <QtWin>


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
    ui->le_output->setText("C:/Output");


    enableControls(false);
    updateSrcControls();

    m_serialPort = new QSerialPort();
    connect(m_serialPort, SIGNAL(readyRead()), SLOT(on_serialPort_data()));

    on_pb_capture_clicked();
 }

MainWindow::~MainWindow()
{
    delete m_serialPort;
    delete ui;
    delete m_capturer;

    GraphFactory::resetInstance();
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

void MainWindow::on_serialPort_data()
{
     QSerialPort::PinoutSignals signs = m_serialPort->pinoutSignals();
     QString data(m_serialPort->readAll());
     //qDebug() << "Data received, pinout signals: " << QString::number(signs) << ", data: " << data;

     QFile f("hauppage.log");
     f.open(QIODevice::Append | QIODevice::Text);
     QTextStream s(&f);
     s  << "Data received, pinout signals: " << QString::number(signs) << ", data: " << data << "\n";
     f.close();

     if (m_capturer->getState() == IGraph::State_Running)
         takePicture();
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
    HBITMAP bmp  = m_capturer->getLastFrame();
    QPixmap pixmap =  QtWin::fromHBITMAP(bmp);
    QString s = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss");
    QFile file(ui->le_output->text() + "\\" + s + ".png");
    file.open(QIODevice::WriteOnly);
    pixmap.save(&file, "png");
}
