#ifndef NPLAYBACK_GRAPH_H
#define NPLAYBACK_GRAPH_H

#include "moreuuids.h"
#include "playback_graph.h"
#include "textoverlay.h"

class NPlaybackGraph : public PlaybackGraph
{
    Q_OBJECT
public:
    NPlaybackGraph(QObject *parent = 0);

protected:
   // Text overlay object
   TextOverlay *m_ovl;

   CComPtr<IBaseFilter> m_ns; //Network source
   CComPtr<IBaseFilter> m_st_v; // Smart tee filter for video branch
   CComPtr<IBaseFilter> m_vd; // Video decoder
   CComPtr<IBaseFilter> m_st_a; // Smart tee filter for audio branch
   CComPtr<IBaseFilter> m_ad; // Audio decoder
   CComPtr<IBaseFilter> m_vr; // Video renderer;

   virtual void reset();
   virtual HRESULT build();
   void addOverlayText(int, int, QString);
   HRESULT applyOverlay();
   void clearOverlay();

};

#endif // NPLAYBACK_GRAPH_H
