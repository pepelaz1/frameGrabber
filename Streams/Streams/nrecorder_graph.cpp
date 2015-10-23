#include "nrecorder_graph.h"

NRecorderGraph::NRecorderGraph(QObject *parent) : BaseGraph(parent)
{
   // qDebug() << "NRecorderGraph::NRecorderGraph";
    m_tovl = 0;
    m_encManager = new EncManager();
    m_encManager->configure(m_cde, m_fm2);

    m_srcManager = new SrcManager();
    m_srcManager->configure(m_cde, m_fm2);

    m_rndManager = new RndManager();
    m_rndManager->configure(m_cde, m_fm2);

    m_ppManager = new PropPageManager();
    m_ppManager->configure(m_cde, m_fm2);
}

NRecorderGraph::~NRecorderGraph()
{
    //m_logger->AddLogMessage(INFO, "CaptureGraph::~CaptureGraph");
    //qDebug() << "NRecorderGraph::~NRecorderGraph";
    reset();
    delete m_srcManager;
    delete m_encManager;
    delete m_rndManager;
    delete m_ppManager;
}

HRESULT NRecorderGraph::addVideoCompressor(CComPtr<IBaseFilter>& filter)
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

HRESULT NRecorderGraph::build()
{
   //   qDebug() << "NRecorderGraph::build() 0";


    HRESULT hr = BaseGraph::build();
    if (FAILED(hr))
        return hr;

    m_encManager->setFilterGraph(m_gb);

   //  qDebug() << "1";

    // Add network source filter
    hr = addFilter(CLSID_LavSource, L"Network Source", m_ns);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to add network source filter hr=" + QString::number(hr));
        return hr;
    }


    // And set source to it
    CComPtr<IFileSourceFilter>fsf;
    hr = m_ns.QueryInterface(&fsf);
    if (FAILED(hr))
    {
        // m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to query IFileSourceFilter interface from network source filter hr=" + QString::number(hr));
        return hr;
    }

    wchar_t* wf = new wchar_t[m_source.size() + 1];
    int n = m_source.toWCharArray(wf);
    wf[n] = 0;

    hr = fsf->Load(wf, NULL);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to set source into network source filter hr=" + QString::number(hr));
        return hr;
    }

    fsf.Release();
    delete wf;



    // Add muxer filter
    if ( m_encManager->getVideoCompressor().contains("MONOGRAM"))
    {
        hr = addFilter(CLSID_MP4Mux, L"MP4 Muxer", m_mux);
        if(FAILED(hr))
        {
            //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to add mp4 muxer filter hr=" + QString::number(hr));
            return hr;
        }
    }
    else
    {
        hr = addFilter(CLSID_AviDest, L"Avi Muxer", m_mux);
        if(FAILED(hr))
        {
            //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to add avi muxer filter hr=" + QString::number(hr));
            return hr;
        }
    }

    //
    // Video branch
    //

    // Add smart tee filter for video branch
    hr = addFilter(CLSID_SmartTee, L"Smart Tee Video", m_st_v);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to add Smart Tee filter for video branch hr=" + QString::number(hr));
        return hr;
    }

    // Connect network source and smart tee video
    hr = connectFilters(m_ns, m_st_v);
    if (FAILED(hr))
    {
         //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed connect network source and smart tee video filters hr=" + QString::number(hr));
         return hr;
    }

    // Add video decoder
    hr = addFilter(CLSID_LavVDec, L"Video Decompressor", m_vd);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to add video decompressor filter hr=" + QString::number(hr));
        return hr;
    }

    // Connect smart tee and video decoder
    hr = connectFilters(m_st_v, m_vd);
    if (FAILED(hr))
    {
         //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to connect smart tee filter and video decoder hr=" + QString::number(hr));
         return hr;
    }

    // Add video compressor filter
    hr = addVideoCompressor(m_vc);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to add video compressor filter hr=" + QString::number(hr));
        return hr;
    }

    // Create and configure text overlay object
    m_tovl = new TextOverlay();
    //m_tovl->configure(m_gb, 0, m_vc);
    m_tovl->setGraphBuilder(m_gb);
    m_tovl->setEncoder(m_vc);


    // Add overlay filter
    hr =  m_tovl->addFilter();
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "CaptureGraph::build(), failed to add overlay filter hr=" + QString::number(hr));
        return hr;
    }

    //qDebug() << "NRecorderGraph::build() 1";

    CComPtr<IBaseFilter> of = m_tovl->getFilter();
    // Connect video decoder and overlay filters
    hr = connectFilters(m_vd, of);
    if (FAILED(hr))
    {
         //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to connect video decoder and overlay filter hr=" + QString::number(hr));
         return hr;
    }

    //  qDebug() << "NRecorderGraph::build() 2";

    // Connect overlay and video compressor filters
    hr = connectFilters(of, m_vc);
    if (FAILED(hr))
    {
         //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to connect overlay filter and video compressor hr=" + QString::number(hr));
         return hr;
    }

    //  qDebug() << "NRecorderGraph::build() 3";

//    // Connect video decoder and video compressor filters
//    hr = connectFilters(m_vd, m_vc);
//    if (FAILED(hr))
//    {
//         m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to connect video decoder and video compressor hr=" + QString::number(hr));
//         return hr;
//    }

    // Connect  video compressor and muxer filter
    hr = connectFilters(m_vc, m_mux);
    if (FAILED(hr))
    {
         //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to connect video compressor and muxer hr=" + QString::number(hr));
         return hr;
    }

     //
    // Audio branch
    //
    // Add smart tee filter for audio branch
    hr = addFilter(CLSID_SmartTee, L"Smart Tee Audio", m_st_a);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to add Smart Tee filter for audio branch hr=" + QString::number(hr));
        return hr;
    }



    // Connect network source and smart tee audio
    hr = connectFilters(m_ns, m_st_a);
    if (SUCCEEDED(hr))
    {
        // Add audio decoder
        hr = addFilter(CLSID_LavADec, L"Audio Decompressor", m_ad);
        if (FAILED(hr))
        {
            //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to add audio decompressor filter hr=" + QString::number(hr));
            return hr;
        }

        // Connect smart tee and audio decoder
        hr = connectFilters(m_st_a, m_ad);
        if (FAILED(hr))
        {
             //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to connect smart tee filter and audio decoder hr=" + QString::number(hr));
             return hr;
        }

          //  qDebug() << "1";

        // Add audio compressor filter
        hr = addAudioCompressor(m_ac);
        if (FAILED(hr))
        {
            //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to add audio compressor filter hr=" + QString::number(hr));
            return hr;
        }

          //  qDebug() << "2";

        // Connect audio decoder and audio encoder
        hr = connectFilters(m_ad, m_ac);
        if (FAILED(hr))
        {
             //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to connect audio decoder and audio compressor filters hr=" + QString::number(hr));
             return hr;
        }

          //  qDebug() << "3";

        // Connect audio compressor and muxer
        hr = connectFilters(m_ac, m_mux);
        if (FAILED(hr))
        {
            // m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to audio compressor and muxer filters hr=" + QString::number(hr));
             return hr;
        }



//        // Connect smart tee and muxer
//        hr = connectFilters(m_st_a, m_mux);
//        if (FAILED(hr))
//        {
//             m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to connect smart tee filter and muxer filters hr=" + QString::number(hr));
//             return hr;
//        }
    // m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed connect network source and smart tee audio filters hr=" + QString::number(hr));
    // return hr;
    }


    // Finish

    // Add file writer filter and set output file name
    hr = addFilter(CLSID_FileWriter, L"File Writer", m_fw);
    if(FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed to add file writer filter hr=" + QString::number(hr));
        return hr;
    }

    wchar_t* w = new wchar_t[m_encManager->getOutputFile().size() + 1];
    n = m_encManager->getOutputFile().toWCharArray(w);
    w[n] = 0;

    CComPtr<IFileSinkFilter> sink;
    m_fw.QueryInterface(&sink);
    sink->SetFileName(w, NULL);


    // Connect mux and writer filter
    hr = connectFilters(m_mux, m_fw);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "NRecorderGraph::build(), failed connect muxer and writer filters hr=" + QString::number(hr));
        return hr;
    }
    return hr;
}

void NRecorderGraph::reset()
{    
    if (m_tovl)
    {
        delete m_tovl;
        m_tovl = 0;
    }

    m_fw.Release();
    m_vc.Release();
    m_ac.Release();
    m_mux.Release();

    m_ns.Release();
    m_st_v.Release();
    m_vd.Release();
    m_st_a.Release();
    m_ad.Release();

    m_as.Release();

    BaseGraph::reset();
}

HRESULT NRecorderGraph::load(QString source, WId)
{
    m_source = source;

    reset();

    return build();
}

void NRecorderGraph::close()
{
    reset();
}


void NRecorderGraph::play()
{
    if (m_mc)
        m_mc->Run();
}

void NRecorderGraph::pause()
{
    if (m_mc)
        m_mc->Pause();
}

void NRecorderGraph::stop()
{
    if (m_mc)
        m_mc->Stop();
}

IGraph::Graph_State NRecorderGraph::getState()
{
    return BaseGraph::getState();
}

__int64 NRecorderGraph::getDuration()
{
    return 0;
}

__int64 NRecorderGraph::getPosition()
{
    return 0;
}

int NRecorderGraph::getFrameNumber()
{
    return 0;
}

void NRecorderGraph::setPosition(__int64)
{

}

void NRecorderGraph::setPosition(int)
{

}

void NRecorderGraph::onSize()
{   
}

void NRecorderGraph::onMove()
{
}

void NRecorderGraph::processEvents()
{
}

void NRecorderGraph::setVolume(short)
{
}

void NRecorderGraph::getVUValues(int, int *, int *)
{
}

HBITMAP NRecorderGraph::getLastFrame()
{
    return 0;
}

void NRecorderGraph::addOverlayText(int x, int y, QString text)
{
    //qDebug() << "NRecorderGraph::addOverlayText";
    if (!m_tovl)
        return;

    m_tovl->add(x, y, text);
}

HRESULT NRecorderGraph::applyOverlay()
{
    if (!m_tovl)
        return S_FALSE;
   return m_tovl->apply();
}

void NRecorderGraph::clearOverlay()
{
   if (m_tovl)
       m_tovl->clear();
}


HRESULT NRecorderGraph::getVideoSources(QStringList &lst)
{
    return m_srcManager->getVideoSources(lst);
}

HRESULT NRecorderGraph::setVideoSource(QString src)
{
    return m_srcManager->setVideoSource(src);
}

HRESULT NRecorderGraph::getAudioSources(QStringList &lst)
{
    return m_srcManager->getAudioSources(lst);
}

HRESULT NRecorderGraph::setAudioSource(QString src)
{
    return m_srcManager->setAudioSource(src);
}

HRESULT NRecorderGraph::getVideoCompressors(QStringList &list)
{
    return m_encManager->getVideoCompressors(list);
}

HRESULT NRecorderGraph::setVideoCompressor(QString enc)
{
   return m_encManager->setVideoCompressor(enc);
}

HRESULT NRecorderGraph::addAudioSource(CComPtr<IBaseFilter>& filter)
{
    return addFilter(m_srcManager->getAudioSource(), L"Audio source", filter, CLSID_AudioInputDeviceCategory);
}


HRESULT NRecorderGraph::addAudioCompressor(CComPtr<IBaseFilter>& filter)
{
    HRESULT hr = S_OK;

    // 1) Try to add filter from audio compressor category
    hr = addFilter(m_encManager->getAudioCompressor(), L"Audio Compressor", filter, CLSID_AudioCompressorCategory);
    if (SUCCEEDED(hr))
        return hr;

    // 2) Try to add filter from montivision audio compressor category
    hr = addFilter(m_encManager->getAudioCompressor(), L"Audio Compressor", filter, CLSID_MontivisionAudioCompressorCategory);
    if (SUCCEEDED(hr))
        return hr;


    // 3) Try DirectShow Filters category in case if we don't find filter in video compressor category
    hr = addFilter(m_encManager->getAudioCompressor(), L"Audio Compressor", filter, CLSID_LegacyAmFilterCategory);

    return hr;
}

HRESULT NRecorderGraph::getAudioCompressors(QStringList &list)
{
    return m_encManager->getAudioCompressors(list);
}

HRESULT NRecorderGraph::setAudioCompressor(QString enc)
{
    return m_encManager->setAudioCompressor(enc);
}

HRESULT NRecorderGraph::setOutputFile(QString file)
{
    return m_encManager->setOutputFile(file);
}

HRESULT NRecorderGraph::getAudioRenderers(QStringList &lst)
{
   return m_rndManager->getAudioRenderers(lst);
}

HRESULT NRecorderGraph::setAudioRenderer(QString arend)
{
   return m_rndManager->setAudioRenderer(arend);
}

HRESULT NRecorderGraph::showVideoWindow(bool)
{
    return S_FALSE;
}

HRESULT NRecorderGraph::getVideoSubtypes(QString sourceName, QStringList &lst)
{
    return m_srcManager->getVideoSubtypes(sourceName, lst);
}

HRESULT NRecorderGraph::getVideoResolutions(QString sourceName, QString subtype, QStringList &lst)
{
    return m_srcManager->getVideoResolutions(sourceName, subtype, lst);
}

HRESULT NRecorderGraph::showPropDialog(QString filterName)
{
    return m_ppManager->showPropDialog(filterName);
}
