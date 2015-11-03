#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSerialPort>
#include <QSettings>

#include "irecordergraph.h"
#include "graphfactory.h"
#include "serialportreader.h"
#include "session.h"
#include "listener.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void listen();
private:
    Ui::MainWindow *ui;
    GraphFactory *m_factory;

    IRecorderGraph *m_capturer;
    QString m_url;
    QString m_dest;
    QString m_vsrc;
    QSerialPort *m_serialPort;
    QSettings *m_settings;
    QString m_settingsFile;
    Listener *m_listener;
    QSerialPort::PinoutSignals  m_pinouts;

    bool rebuildCap();
    void populateDevices();
    void populatePorts();
    void updateSrcControls();
    void enableControls(bool);
    void overlayText(IGraph *);
    void takePicture();
    void loadSettings();
    void saveSettings();
    void writeLog(QString);
    void initListener();

protected:
    virtual void closeEvent (QCloseEvent *);
private slots:
    // serial port slots
    void on_readyRead();
    void on_baudRateChanged(qint32, QSerialPort::Directions);
    void on_breakEnabledChanged(bool);
    void on_dataBitsChanged(QSerialPort::DataBits);
    void on_dataTerminalReadyChanged(bool);
    void on_error(QSerialPort::SerialPortError);
    void on_flowControlChanged(QSerialPort::FlowControl);
    void on_parityChanged(QSerialPort::Parity);
    void on_requestToSendChanged(bool);
    void on_stopBitsChanged(QSerialPort::StopBits);

    void on_quant();

    void on_pb_capture_clicked();
    void on_pb_browse_clicked();
    void on_pb_take_picture_clicked();
    void on_cb_listenport_currentIndexChanged(int);

};

#endif // MAINWINDOW_H
