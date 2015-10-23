#ifndef GRAPH_H
#define GRAPH_H

#include "Streams_global.h"
#include <QObject>

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>

#include <QString>
#include "igraph.h"
#include <atlbase.h>
#include <InitGuid.h>
#include <streams.h>
#include <strmif.h>
//#include "logger.h"
#include "vumeter.h"
#include "videowindow.h"
#include <qedit.h>
#include "base_graph.h"

class PlaybackGraph : public BaseGraph, public IGraph
{
    Q_OBJECT
 public:
    PlaybackGraph(QObject *parent = 0);
     ~PlaybackGraph();
    HRESULT load(QString, WId);
    void close();
    void play();
    void pause();
    void stop();
    Graph_State getState();
    __int64 getDuration();
    __int64 getPosition();
    int getFrameNumber();
    void setPosition(__int64);
    void setPosition(int);
    void onSize();
    void onMove();
    void processEvents();
    void setVolume(short);
    void getVUValues(int, int *, int *);
    HBITMAP getLastFrame();
    void addOverlayText(int, int, QString);
    HRESULT applyOverlay();
    void clearOverlay();
    HRESULT getAudioRenderers(QStringList &);
    HRESULT setAudioRenderer(QString);
    HRESULT showVideoWindow(bool);

   // void OnRepaint();
protected:
//    //Owner window
    WId m_wnd;
    // VU meter
    VUMeter *m_vu;
    // Video window
    VideoWindow *m_vw;

    // Filters
    CComPtr<IBaseFilter>m_src;
    CComPtr<IBaseFilter>m_ar;
    CComPtr<IBaseFilter>m_asg;

    // Interfaces
    CComPtr<ISampleGrabber> m_sg;
    CComPtr<IBasicAudio> m_ba;

    void configureVideoWindow();
    void updateVideoSize();
    void onComplete();
    HRESULT insertAudioGrabber();

 protected:
    virtual void reset();
    virtual HRESULT build();
};

#endif // GRAPH_H
