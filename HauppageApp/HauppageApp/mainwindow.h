#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSerialPort>

#include "irecordergraph.h"
#include "graphfactory.h"
#include "serialportreader.h"
//#include "logger.h"

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

    bool rebuildCap();
    void populateDevices();
    void populatePorts();
    void updateSrcControls();
    void enableControls(bool);
    void overlayText(IGraph *);
    void takePicture();
private slots:   
    void on_serialPort_data();
    void on_pb_capture_clicked();
    void on_pb_browse_clicked();
    void on_pb_take_picture_clicked();
    void on_cb_listenport_currentIndexChanged(int);
};

#endif // MAINWINDOW_H
