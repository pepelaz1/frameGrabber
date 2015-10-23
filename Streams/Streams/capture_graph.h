#ifndef CAPTURE_GRAPH_H
#define CAPTURE_GRAPH_H

#include "irecordergraph.h"
#include <QObject>
#include <Windows.h>
#include <videowindow.h>
#include "base_graph.h"
#include "moreuuids.h"
#include "textoverlay.h"
#include "encmanager.h"
#include "srcmanager.h"
#include "rndmanager.h"
#include "proppagemanager.h"

class CaptureGraph : public BaseGraph, public IRecorderGraph
{
    Q_OBJECT
public:
    CaptureGraph(QObject *parent = 0);
    virtual ~CaptureGraph();
    
    // IGraph implementation
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

    // IRecorderGraph implementation
    HRESULT getVideoSources(QStringList &);
    HRESULT setVideoSource(QString);
    HRESULT getAudioSources(QStringList &);
    HRESULT setAudioSource(QString);

    HRESULT getVideoCompressors(QStringList &);
    HRESULT setVideoCompressor(QString);
    HRESULT getAudioCompressors(QStringList &);
    HRESULT setAudioCompressor(QString);
    HRESULT setOutputFile(QString);

    HRESULT getVideoSubtypes(QString, QStringList &);
    HRESULT getVideoResolutions(QString, QString, QStringList &);

    HRESULT showPropDialog(QString);
signals:
    
public slots:

private:

   //Owner window for preview
   WId m_wnd;

    // Video window
   VideoWindow *m_vw;

   // Encoding manager object
   EncManager *m_encManager;

   // Source manager object
   SrcManager *m_srcManager;

   // Renderer manager object
   RndManager *m_rndManager;

   // Property pages manager object
   PropPageManager *m_ppManager;

   // Text overlay object
   TextOverlay *m_tovl;

   // Interfaces
   CComPtr<ICaptureGraphBuilder2> m_cgb;


   // Filters
   CComPtr<IBaseFilter> m_fw; // File writer
   CComPtr<IBaseFilter> m_vs; // Video source
   CComPtr<IBaseFilter> m_vc; // Video compressor
   CComPtr<IBaseFilter> m_as; // Audio source
   CComPtr<IBaseFilter> m_ac; // Audio compressor
   CComPtr<IBaseFilter> m_mux; // Muxer
   CComPtr<IBaseFilter> m_vr; // Video renderer;


   HRESULT addVideoSource(CComPtr<IBaseFilter>&);
   HRESULT addVideoCompressor(CComPtr<IBaseFilter>&);
   void updateVideoSize();

   HRESULT addAudioSource(CComPtr<IBaseFilter>&);
   HRESULT addAudioCompressor(CComPtr<IBaseFilter>&);

protected:
   virtual void reset();
   virtual HRESULT build();    
};

#endif // CAPTURE_GRAPH_H
