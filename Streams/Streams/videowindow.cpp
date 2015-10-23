#include "videowindow.h"
#include <QDebug>

VideoWindow::VideoWindow(QObject *parent) :
    QObject(parent)
{
}

VideoWindow::~VideoWindow()
{
   m_vw.Release();
   m_bv.Release();
}

HRESULT VideoWindow::configure(CComPtr<IGraphBuilder> gb, WId wnd)
{
    HRESULT hr = S_OK;

    //m_logger = Logger::GetInstance();

    m_gb = gb;
    m_wnd = wnd;

    // Query IVideoWindow interface
    hr = m_gb.QueryInterface(&m_vw);
    if(FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "CVideoWindow::setFilterGraph(), failed to query IVideoWindow interface hr=" + QString::number(hr));
        return hr;
    }

    // Query IBasicVideo interface
    hr = m_gb.QueryInterface(&m_bv);
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "CVideoWindow::setFilterGraph(), failed to query IBasicVideo interface hr=" + QString::number(hr));
        return hr;
    }

    m_vw->put_Owner(0);
    m_vw->put_Owner((OAHWND)m_wnd);
    m_vw->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
    updateSize();

    return hr;
}

HRESULT VideoWindow::updateSize()
{
    if (m_vw && m_bv)
    {
        long width, height;
        m_bv->get_VideoWidth(&width);
        m_bv->get_VideoHeight(&height);

        RECT r;
        GetClientRect((HWND)m_wnd, &r);

       // qDebug() << "r.right=" << r.right << " width=" << width << " r.bottom=" << r.bottom << " height=" << height;

        double k = ((double)height/(double)width);
        if ( r.right > r.bottom / k)
        {
            int w1 = r.bottom  / k;
            m_vw->SetWindowPosition((r.right - w1)/2, 0, w1, r.bottom);
        }
        else
        {
            int h1 = r.right * k;
            m_vw->SetWindowPosition(0, (r.bottom - h1)/2, r.right, h1);

        }
     }
    return S_OK;
}

HRESULT VideoWindow::updateMove()
{
    if (m_vw)
        m_vw->NotifyOwnerMessage((OAHWND)m_wnd,WM_MOVE,0,0);
    return S_OK;
}


HRESULT VideoWindow::getAvgTimePerFrame(REFTIME *rt)
{
    return m_bv->get_AvgTimePerFrame(rt);
}




HBITMAP VideoWindow::getLastFrame()
{
    long size;

    //HRESULT hr = m_bv->GetCurrentImage(&size, NULL);
    Sleep(50);

    HRESULT hr = m_bv->GetCurrentImage(&size, NULL);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(INFO, "VideoWindow::getLastFrame(), failed to get image size, hr=" +   QString::number(hr));
        return 0;
    }

    long *buffer = (long *)new unsigned char[size];
    hr = m_bv->GetCurrentImage(&size, buffer);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(INFO, "VideoWindow::getLastFrame(), failed to get image buffer, hr=" +   QString::number(hr));
        return 0;
    }
    //m_logger->AddLogMessage(INFO, "PlaybackGraph::getLastFrame(), size=" +  QString::number(size) + " hr=" + QString::number(hr));

    HDC hdc = GetDC(NULL);
    BITMAPINFO *bmi = (BITMAPINFO *)buffer;
    const BYTE* bits = (const BYTE*) (&bmi->bmiHeader + 1);

    UINT * pPixels = 0;
    HBITMAP hbmp = CreateDIBSection( hdc, (BITMAPINFO *)
                                bmi, DIB_RGB_COLORS, (void **)&
                                pPixels  , NULL, 0);


    if ( hbmp )
        memcpy(pPixels, bits , bmi->bmiHeader.biSizeImage  );

    ReleaseDC(NULL, hdc);
    delete buffer;
    return hbmp;
}

HRESULT VideoWindow::showWindow(bool show)
{
   // qDebug() << "VideoWindow::showWindow show = " << QString::number(show);
    if (m_vw)
        m_vw->put_Visible(show ? OATRUE : OAFALSE);
   // ShowWindow(m_wnd, show ? SW_SHOW : SW_HIDE);
    return S_OK;
}
