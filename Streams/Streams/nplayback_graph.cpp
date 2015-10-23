#include "nplayback_graph.h"

NPlaybackGraph::NPlaybackGraph(QObject *parent) : PlaybackGraph(parent)
{
    m_ovl = 0;
    //qDebug() << "NPlaybackGraph::NPlaybackGraph";
}


void NPlaybackGraph::reset()
{
    if (m_ovl)
    {
        delete m_ovl;
        m_ovl = 0;
    }

    m_ns.Release();
    m_st_v.Release();
    m_vd.Release();
    m_st_a.Release();
    m_ad.Release();
    PlaybackGraph::reset();    

}

HRESULT NPlaybackGraph::build()
{
    //qDebug() << "NPlaybackGraph::build";
    HRESULT hr = BaseGraph::build();
    if (FAILED(hr))
        return hr;

    // Add network source filter
    hr = addFilter(CLSID_LavSource, L"Network Source", m_ns);
    if (FAILED(hr))
    {
        //qDebug() << "NPlaybackGraph::build(), failed to set add network source filter hr=" << QString::number(hr);
        //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to set add network source filter hr=" + QString::number(hr));
        return hr;
    }

    // And set source to it
    CComPtr<IFileSourceFilter>fsf;
    hr = m_ns.QueryInterface(&fsf);
    if (FAILED(hr))
    {
        //qDebug() << "NPlaybackGraph::build(), failed to query IFileSourceFilter interface from network source filter hr=" << QString::number(hr);
        //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to query IFileSourceFilter interface from network source filter hr=" + QString::number(hr));
        return hr;
    }

    wchar_t* wf = new wchar_t[m_source.size() + 1];
    int n = m_source.toWCharArray(wf);
    wf[n] = 0;
   // qDebug() << "m_source " << m_source;
    hr = fsf->Load(wf, NULL);
    if (FAILED(hr))
    {
        //qDebug() << "NPlaybackGraph::build(), failed to set source into network source filter hr=" << QString::number(hr);
        // m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to set source into network source filter hr=" + QString::number(hr));
        return hr;
    }

    fsf.Release();
    delete wf;


    //
    // Video branch
    //

    // Add smart tee filter for video branch
    hr = addFilter(CLSID_SmartTee, L"Smart Tee Video", m_st_v);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to add Smart Tee filter for video branch hr=" + QString::number(hr));
        return hr;
    }

    // Connect network source and smart tee video
    hr = connectFilters(m_ns, m_st_v);
    if (FAILED(hr))
    {
         //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed connect network source and smart tee video filters hr=" + QString::number(hr));
         return hr;
    }

    // Add video decoder
    hr = addFilter(CLSID_LavVDec, L"Video Decompressor", m_vd);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to add video decompressor filter hr=" + QString::number(hr));
        return hr;
    }

    // Connect smart tee and video decoder
    hr = connectFilters(m_st_v, m_vd);
    if (FAILED(hr))
    {
         //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to connect smart tee filter and video decoder hr=" + QString::number(hr));
         return hr;
    }

    // Add video renderer filter
    hr = addFilter(CLSID_VideoMixingRenderer9, L"Video Renderer", m_vr);
    if(FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to add video renderer filter hr=" + QString::number(hr));
        return hr;
    }

    // Connect video decoder and video renderer
    hr = connectFilters(m_vd, m_vr);
    if (FAILED(hr))
    {
         //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to connect video decoder and video rendere hr=" + QString::number(hr));
         return hr;
    }


//    hr = renderFilter(m_vd);
//    if (FAILED(hr))
//    {
//        m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to render video decoder hr=" + QString::number(hr));
//        return hr;
//    }

    m_vw = new VideoWindow();
    hr = m_vw->configure(m_gb, m_wnd);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to set graph builder object into video window object hr=" + QString::number(hr));
        return hr;
    }


    // Create and configure text overlay object
    m_ovl = new TextOverlay();
    //m_ovl->configure(m_gb, m_vr, 0);
    m_ovl->setGraphBuilder(m_gb);
    m_ovl->setRenderer(m_vr);
    //
    // Audio branch
    //
    // Add smart tee filter for audio branch
    hr = addFilter(CLSID_SmartTee, L"Smart Tee Audio", m_st_a);
    if (FAILED(hr))
    {
        // m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to add Smart Tee filter for audio branch hr=" + QString::number(hr));
        return hr;
    }

    // Connect network source and smart tee audio
    // Consider that possible http/rtsp streams without audio
    hr = connectFilters(m_ns, m_st_a);
    if (SUCCEEDED(hr))
    {
        // Add audio decoder
        hr = addFilter(CLSID_LavADec, L"Audio Decompressor", m_ad);
        if (FAILED(hr))
        {
            //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to add audio decompressor filter hr=" + QString::number(hr));
            return hr;
        }

        // Connect smart tee and audio decoder
        hr = connectFilters(m_st_a, m_ad);
        if (FAILED(hr))
        {
             //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to connect smart tee filter and audio decoder hr=" + QString::number(hr));
             return hr;
        }

        // Render audio branch
        hr = renderFilter(m_ad);
        if (FAILED(hr))
        {
            //m_logger->AddLogMessage(CRITICAL, "NPlaybackGraph::build(), failed to render audio decoder hr=" + QString::number(hr));
            return hr;
        }
    }

    return S_OK;
}


void NPlaybackGraph::addOverlayText(int x, int y, QString text)
{
    //qDebug() << "NPlaybackGraph::addOverlayText";
    if (m_ovl)
        m_ovl->add(x, y, text);
}

HRESULT NPlaybackGraph::applyOverlay()
{
    if (!m_ovl)
        return S_FALSE;
    return m_ovl->apply();
}

void NPlaybackGraph::clearOverlay()
{
    if(m_ovl)
        m_ovl->clear();
}

