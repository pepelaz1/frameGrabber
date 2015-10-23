#ifndef IGRAPH_H
#define IGRAPH_H

#include "Streams_global.h"
#include <QString>
#include <QImage>

class STREAMSSHARED_EXPORT IGraph
{
public:
    enum Graph_State
        {  State_Stopped	= 0,
           State_Paused	= ( State_Stopped + 1 ) ,
           State_Running	= ( State_Paused + 1 )
         };
    virtual ~IGraph() {}
    virtual HRESULT load(QString, WId) = 0;
    virtual void close() = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual Graph_State getState() = 0;
    virtual __int64 getDuration() = 0;
    virtual __int64 getPosition() = 0;
    virtual void setPosition(__int64) = 0;
    virtual void setPosition(int) = 0;
    virtual void onSize() = 0;
    virtual void onMove() = 0;
    virtual void processEvents() = 0;
    virtual void setVolume(short) = 0;
    virtual void getVUValues(int, int *, int *) = 0;
    virtual HBITMAP getLastFrame() = 0;
    virtual void addOverlayText(int, int, QString) = 0;
    virtual HRESULT applyOverlay() = 0;
    virtual void clearOverlay() = 0;

    virtual HRESULT getAudioRenderers(QStringList &) = 0;
    virtual HRESULT setAudioRenderer(QString) = 0;

    virtual HRESULT showVideoWindow(bool) = 0;

    // obsolete
    virtual int getFrameNumber() = 0;
};


#endif // IGRAPH_H
