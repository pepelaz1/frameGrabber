#pragma once
#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QObject>
#include <QtWidgets>
#include <atlbase.h>
#include <streams.h>
#include "InitGuid.h"
//#include "logger.h"

class VideoWindow : public QObject
{
    Q_OBJECT
public:
    explicit VideoWindow(QObject *parent = 0);
    ~VideoWindow();
    HRESULT configure(CComPtr<IGraphBuilder>, WId );
    HRESULT updateSize();
    HRESULT updateMove();
    HRESULT getAvgTimePerFrame(REFTIME *);
    HBITMAP getLastFrame();
    HRESULT showWindow(bool);
signals:
    
public slots:

private:
    //Owner window
    WId m_wnd;
    // Logger object
    //Logger *m_logger;

    CComPtr<IGraphBuilder> m_gb;
    CComPtr<IVideoWindow> m_vw;
    CComPtr<IBasicVideo> m_bv;

};

#endif // VIDEOWINDOW_H
