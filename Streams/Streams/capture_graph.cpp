#include "capture_graph.h"

CaptureGraph::CaptureGraph(QObject *parent) : BaseGraph(parent)
{
    m_vw = 0;
    m_tovl = 0;
    //qDebug() << "CaptureGraph::CaptureGraph";

    m_encManager = new EncManager();
    m_encManager->configure(m_cde, m_fm2);

    m_srcManager = new SrcManager();
    m_srcManager->configure(m_cde, m_fm2);

    m_rndManager = new RndManager();
    m_rndManager->configure(m_cde, m_fm2);

    m_ppManager = new PropPageManager();
    m_ppManager->configure(m_cde, m_fm2);
}

CaptureGraph::~CaptureGraph()
{
    //m_logger->AddLogMessage(INFO, "CaptureGraph::~CaptureGraph");
    //qDebug() << "CaptureGraph::~CaptureGraph";
    delete m_encManager;
    delete m_srcManager;
    delete m_rndManager;
    delete m_ppManager;
    reset();
}

HRESULT CaptureGraph::addVideoSource(CComPtr<IBaseFilter>& filter)
{
    //m_logger->AddLogMessage(INFO, "CaptureGraph::addVideoSource = " + m_srcManager->getVideoSource());
    return addFilter(m_srcManager->getVideoSource(), L"Video source", filter, CLSID_VideoInputDeviceCategory);
}

HRESULT CaptureGraph::addVideoCompressor(CComPtr<IBaseFilter>& filter)
{
    HRESULT hr = S_OK;

    // 1) Try to add filter from video compressor category
    hr = addFilter(m_encManager->getVideoCompressor(), L"Video Compressor", filter, CLSID_VideoCompressorCategory);
    if (SUCCEEDED(hr))
        return hr;

    // 2) Try DirectShow Filters category in case if we don't find filter in video compressor category
    hr = addFilter(m_encManager->getVideoCompressor(), L"Video Compressor", filter, CLSID_LegacyAmFilterCategory);

    return hr;
}

HRESULT CaptureGraph::build()
{
    //qDebug() << "CaptureGraph::build";

    HRESULT hr = BaseGraph::build();
    if (FAILED(hr))
        return hr;

    // Create capture graph builder object
    hr = m_cgb.CoCreateInstance(CLSID_CaptureGraphBuilder2);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to create capture graph builder object hr=" + QString::number(hr));
        return hr;
    }


    // Set filter graph
    hr = m_cgb->SetFiltergraph(m_gb);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to set graph builder object into capture graph builder object hr=" + QString::number(hr));
        return hr;
    }


    // Add video source filter
    hr = addVideoSource(m_vs);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to set add video capture filter hr=" + QString::number(hr));
        return hr;
    }

    m_tovl = new TextOverlay();
    m_tovl->setGraphBuilder(m_gb);


    // Add video renderer filter
    hr = addFilter(CLSID_VideoMixingRenderer9, L"Video Renderer", m_vr);
    if(FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to add video renderer filter hr=" + QString::number(hr));
        return hr;
    }
    m_tovl->setRenderer(m_vr);

    hr = m_cgb->RenderStream(
        &PIN_CATEGORY_PREVIEW, // Pin category.
        &MEDIATYPE_Video,      // Media type.
        m_vs,                  // Capture filter.
        NULL,                  // Intermediate filter (optional).
        m_vr);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to render video preview branch hr=" + QString::number(hr));
        return hr;
    }


    m_vw = new VideoWindow();
    hr = m_vw->configure(m_gb, m_wnd);
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to set graph builder object into video window object hr=" + QString::number(hr));
        return hr;
    }


    if (m_encManager->getOutputFile() != "")
    {
        // If output file is set, then build capture branch as well

        // Video part

        // Add video compressor filter
        hr = addVideoCompressor(m_vc);
        if (FAILED(hr))
        {          
           // m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to set add video compressor filter hr=" + QString::number(hr));
            return hr;
        }
        m_tovl->setEncoder(m_vc);

        // Add muxer filter
        if ( m_encManager->getVideoCompressor().contains("MONOGRAM"))
        {
            hr = addFilter(CLSID_MP4Mux, L"MP4 Muxer", m_mux);
            if(FAILED(hr))
            {
                //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to add mp4 muxer filter hr=" + QString::number(hr));
                return hr;
            }
        }
        else
        {
            hr = addFilter(CLSID_AviDest, L"Avi Muxer", m_mux);
            if(FAILED(hr))
            {
               // m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to add avi muxer filter hr=" + QString::number(hr));
                return hr;
            }
        }

        hr =  m_tovl->addFilter();
        if (FAILED(hr))
        {
            //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to add overlay filter hr=" + QString::number(hr));
            return hr;
        }


        // Render video capture branch
        hr = m_cgb->RenderStream(
            &PIN_CATEGORY_CAPTURE, // Pin category.
            &MEDIATYPE_Video,      // Media type.
            m_vs,                  // Capture filter.
            m_tovl->getFilter(),                  // Intermediate filter (optional).
            m_mux);
        if (FAILED(hr))
        {
            //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to render video capture branch hr=" + QString::number(hr));
            return hr;
        }


        // Add filer write filter and set output file name
        hr = addFilter(CLSID_FileWriter, L"File Writer", m_fw);
        if(FAILED(hr))
        {
            //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to add file writer filter hr=" + QString::number(hr));
            return hr;
        }

        wchar_t* w = new wchar_t[m_encManager->getOutputFile().size() + 1];
        int n = m_encManager->getOutputFile().toWCharArray(w);
        w[n] = 0;

        CComPtr<IFileSinkFilter> sink;
        m_fw.QueryInterface(&sink);
        sink->SetFileName(w, NULL);

        // Connect mux and writer filter
        hr = connectFilters(m_mux, m_fw);
        if (FAILED(hr))
        {
            //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed connect muxer and writer filters hr=" + QString::number(hr));
            return hr;
        }


        // Audio part
        if (m_srcManager->getAudioSource() != "")
        {
            // Add audio source filter
            hr = addAudioSource(m_as);
            if (FAILED(hr))
            {
                //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to set add audio capture filter hr=" + QString::number(hr));
                return hr;
            }

            // Add audio compressor filter
            hr = addAudioCompressor(m_ac);
            if (FAILED(hr))
            {
                //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to set add audio compressor filter hr=" + QString::number(hr));
                return hr;
            }

            // Render audio capture branch
            hr = m_cgb->RenderStream(
                &PIN_CATEGORY_CAPTURE, // Pin category.
                &MEDIATYPE_Audio,      // Media type.
                m_as,                  // Capture filter.
                m_ac,                  // Intermediate filter (optional).
                m_mux);
            if (FAILED(hr))
            {
                //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to render audio capture branch hr=" + QString::number(hr));
                return hr;
            }
        }
    }

    return hr;
}

void CaptureGraph::reset()
{

    if (m_vw)
    {
       delete m_vw;
       m_vw = 0;
    }

    if (m_tovl)
    {
        delete m_tovl;
        m_tovl = 0;
    }

    m_fw.Release();
    m_vs.Release();
    m_vc.Release();
    m_as.Release();
    m_ac.Release();
    m_mux.Release();
    m_vr.Release();
    m_cgb.Release();

    BaseGraph::reset();
}

HRESULT CaptureGraph::load(QString, WId wnd)
{
    m_wnd = wnd;

    reset();

    return build();
}

void CaptureGraph::close()
{
    reset();
}


void CaptureGraph::play()
{
    if (m_mc)
        m_mc->Run();
}

void CaptureGraph::pause()
{
    if (m_mc)
        m_mc->Pause();
}

void CaptureGraph::stop()
{
    if (m_mc)
        m_mc->Stop();
}

IGraph::Graph_State CaptureGraph::getState()
{
    return BaseGraph::getState();
}

__int64 CaptureGraph::getDuration()
{
    return 0;
}

__int64 CaptureGraph::getPosition()
{
    return 0;
}

int CaptureGraph::getFrameNumber()
{
    return 0;
}

void CaptureGraph::setPosition(__int64)
{

}

void CaptureGraph::setPosition(int)
{

}

void CaptureGraph::onSize()
{
    if (m_vw)
        m_vw->updateSize();
}

void CaptureGraph::onMove()
{
    if (m_vw)
        m_vw->updateMove();
}

void CaptureGraph::processEvents()
{

}

void CaptureGraph::setVolume(short)
{

}

void CaptureGraph::getVUValues(int, int *, int *)
{

}

HBITMAP CaptureGraph::getLastFrame()
{
    if (!m_vw)
        return 0;

    return m_vw->getLastFrame();
}

void CaptureGraph::addOverlayText(int x, int y, QString text)
{
    if (!m_tovl)
        return;

    m_tovl->add(x, y, text);
}

HRESULT CaptureGraph::applyOverlay()
{
    if (!m_tovl)
        return S_FALSE;

    return m_tovl->apply();
}

void CaptureGraph::clearOverlay()
{
    if (!m_tovl)
        return;

    m_tovl->clear();
}

void CaptureGraph::updateVideoSize()
{
    if (m_vw)
        m_vw->updateSize();
}

HRESULT CaptureGraph::getVideoSources(QStringList &lst)
{
    return m_srcManager->getVideoSources(lst);
}

HRESULT CaptureGraph::setVideoSource(QString src)
{
    return m_srcManager->setVideoSource(src);
}

HRESULT CaptureGraph::getAudioSources(QStringList &lst)
{
    return m_srcManager->getAudioSources(lst);
}

HRESULT CaptureGraph::setAudioSource(QString src)
{
    return m_srcManager->setAudioSource(src);
}

HRESULT CaptureGraph::getVideoCompressors(QStringList &list)
{
   return m_encManager->getVideoCompressors(list);
}

HRESULT CaptureGraph::setVideoCompressor(QString enc)
{
   return m_encManager->setVideoCompressor(enc);
}

HRESULT CaptureGraph::addAudioSource(CComPtr<IBaseFilter>& filter)
{
    return addFilter(m_srcManager->getAudioSource(), L"Audio source", filter, CLSID_AudioInputDeviceCategory);
}

HRESULT CaptureGraph::addAudioCompressor(CComPtr<IBaseFilter>& filter)
{
    HRESULT hr = S_OK;

    // 1) Try to add filter from audio compressor category
    hr = addFilter(m_encManager->getAudioCompressor(), L"Audio Compressor", filter, CLSID_AudioCompressorCategory);
    if (SUCCEEDED(hr))
        return hr;

    // 2) Try DirectShow Filters category in case if we don't find filter in video compressor category
    hr = addFilter(m_encManager->getAudioCompressor(), L"Audio Compressor", filter, CLSID_LegacyAmFilterCategory);

    return hr;
}


HRESULT CaptureGraph::getAudioCompressors(QStringList &list)
{
    return m_encManager->getAudioCompressors(list);
}

HRESULT CaptureGraph::setAudioCompressor(QString enc)
{
    return m_encManager->setAudioCompressor(enc);
}

HRESULT CaptureGraph::setOutputFile(QString file)
{
    return m_encManager->setOutputFile(file);
}

HRESULT CaptureGraph::getAudioRenderers(QStringList &lst)
{
    return m_rndManager->getAudioRenderers(lst);
}

HRESULT CaptureGraph::setAudioRenderer(QString arend)
{
    return m_rndManager->setAudioRenderer(arend);
}

HRESULT CaptureGraph::showVideoWindow(bool show)
{
    HRESULT hr = S_FALSE;
    if (m_vw)
       hr = m_vw->showWindow(show);
    return hr;
}

HRESULT CaptureGraph::getVideoSubtypes(QString sourceName, QStringList &lst)
{
    return m_srcManager->getVideoSubtypes(sourceName, lst);
}

HRESULT CaptureGraph::getVideoResolutions(QString sourceName, QString subtype, QStringList &lst)
{
    return m_srcManager->getVideoResolutions(sourceName, subtype, lst);
}

HRESULT CaptureGraph::showPropDialog(QString filterName)
{
    return m_ppManager->showPropDialog(filterName);
}
