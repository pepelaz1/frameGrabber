#ifndef VUMETER_H
#define VUMETER_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <streams.h>
#include <strmif.h>
#include "qedit.h"
#include <InitGuid.h>
//#include "logger.h"

class VUMeter : public QObject, ISampleGrabberCB
{
    Q_OBJECT
public:
    explicit VUMeter(QObject *parent = 0);
    ~VUMeter();
    

    void setGrabber(ISampleGrabber *);
    void getValues(int, int *, int *);
signals:
public slots:
private slots:
  void on_timer();

private:
    ISampleGrabber *m_sg;
    //Logger *m_logger;
    int m_avgR, m_avgL, m_peakR, m_peakL;
    QTimer *m_timer;
    int m_prev, m_cnt;

    void resetValues();
public:
    // IUnkwnown implementation
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();
    HRESULT STDMETHODCALLTYPE  QueryInterface(REFIID, void **);

    // ISampleGrabberCB implementation
    STDMETHODIMP SampleCB(double, IMediaSample *);
    STDMETHODIMP BufferCB(double, BYTE *, long );

    void onPause();
    void onStop();
};

#endif // VUMETER_H
