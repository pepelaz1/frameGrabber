#include "playback_graph.h"
#include <Windows.h>
#include <QDebug>
#include "qedit.h"
#include "strsafe.h"


PlaybackGraph::PlaybackGraph(QObject *parent) : BaseGraph(parent)
{
   //qDebug() << "PlaybackGraph::PlaybackGraph";
   m_vu = new VUMeter();   
   m_vw = 0;  
}

PlaybackGraph::~PlaybackGraph()
{
   //m_logger->AddLogMessage(INFO, "PlaybackGraph::~PlaybackGraph");
   reset();
   delete m_vu;
}

HRESULT PlaybackGraph::load(QString fileName, WId wnd)
{
    //HRESULT hr = S_OK;
    //m_logger->AddLogMessage(INFO, "PlaybackGraph::load(), Loading file: " + fileName);

    m_source = fileName;
    m_wnd = wnd;

    // Reset
    reset();

    // Build
    return build();
}

void PlaybackGraph::close()
{
    reset();
}

HRESULT PlaybackGraph::build()
{
    HRESULT hr = BaseGraph::build();
    if (FAILED(hr))
        return hr;


    // Add source filter and put input file to it
    hr = addFilter(CLSID_AsyncReader, L"File source", m_src);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::build(), failed to add source filter hr=" + QString::number(hr));
        return hr;
    }

    CComPtr<IFileSourceFilter>fsf;
    hr = m_src.QueryInterface(&fsf);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::build(), failed to query IFileSourceFilter interface from source filter hr=" + QString::number(hr));
        return hr;
    }

    wchar_t* wf = new wchar_t[m_source.size() + 1];
    int n = m_source.toWCharArray(wf);
    wf[n] = 0;

    hr = fsf->Load(wf, NULL);
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::build(), failed to load file into source filter hr=" + QString::number(hr));
        return hr;
    }

    fsf.Release();
    delete wf;

        //  Add video renderer
    //    hr = addFilter(CLSID_VideoRendererDefault, L"Video Renderer", m_vr);
    //    if (FAILED(hr))
    //    {
    //        m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::build(), failed to add video renderer hr=" + QString::number(hr));
    //        return hr;
    //    }

    //  Add audio renderer
    hr = addFilter(CLSID_DSoundRender, L"Audio Renderer", m_ar);
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::build(), failed to add audio renderer hr=" + QString::number(hr));
        return hr;
    }

    // Render source filter's output pin
    CComPtr<IEnumPins> enm;
    CComPtr<IPin> pin;
    m_src->EnumPins(&enm);
    enm->Next(1, &pin, 0);
    enm.Release();

    hr = m_gb->Render(pin);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::build(), failed to render source filter's output pin hr=" + QString::number(hr));
        return hr;
    }

    pin.Release();

    hr = insertAudioGrabber();
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::build(), failed to insert and configure audio grabber hr=" + QString::number(hr));
        return hr;
    }

    // Query IBasicAudio interface
    hr = m_gb.QueryInterface(&m_ba);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::build(), failed to query IBasicAudio interface hr=" + QString::number(hr));
        return hr;
    }

    m_vw = new VideoWindow();
    hr = m_vw->configure(m_gb, m_wnd);
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::build(), failed to set graph builder object into video window object hr=" + QString::number(hr));
        return hr;
    }
    return hr;
}

void PlaybackGraph::reset()
{
    if (m_vw)
    {
       delete m_vw;
       m_vw = 0;
    }

    m_ar.Release();
    m_src.Release();
    m_asg.Release();

    m_ba.Release();
    m_sg.Release();

    BaseGraph::reset();
}


HRESULT PlaybackGraph::insertAudioGrabber()
{
    HRESULT hr = S_OK;

    // Get audio renderer input pin
    CComPtr<IEnumPins> enm;
    CComPtr<IPin> pin;
    m_ar->EnumPins(&enm);
    enm->Next(1, &pin, 0);
    enm.Release();

    // Get connected pin of this pin (audio decoder's pin)
    CComPtr<IPin> connected;
    hr = pin->ConnectedTo(&connected);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::insertAudioGrabber(), Failed to get connected pin to audio renderer hr=" + QString::number(hr));
        return hr;
    }


    // Disconnect audio renderer's output pin
    m_gb->Disconnect(connected);
    m_gb->Disconnect(pin);


    // Add audio grabber filter
    hr = addFilter(CLSID_SampleGrabber, L"Audio Grabber", m_asg);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::insertAudioGrabber(), failed to add audio grabber hr=" + QString::number(hr));
        return hr;
    }

    // Connect audio decoder's pin to audio grabber filter
    hr = connectOutputPinToFilter(connected, m_asg);
    if (FAILED(hr))
    {
       // m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::insertAudioGrabber(), failed to connect audio decoder's output pin to audio grabber filter' hr=" + QString::number(hr));
        return hr;
    }

    // Connect audio grabber to audio renderer
    hr = connectFilters(m_asg, m_ar);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::insertAudioGrabber(), failed to connect audio grabber to audio renderer' hr=" + QString::number(hr));
        return hr;
    }

    connected.Release();
    pin.Release();

    //    // Configure audio grabber
    //    CComPtr<ISampleGrabber> sg;
     hr = m_asg->QueryInterface(IID_ISampleGrabber, (void **)&m_sg);
     if (FAILED(hr))
     {
        // m_logger->AddLogMessage(CRITICAL, "PlaybackGraph::insertAudioGrabber(), failed to query ISampleGrabber interface' hr=" + QString::number(hr));
         return hr;
     }
    //    qDebug() << "sg=" << sg;
    //    AM_MEDIA_TYPE amt;
    //    ZeroMemory(&amt, sizeof(AM_MEDIA_TYPE));
    //    amt.majortype = MEDIATYPE_Audio;
    //    amt.subtype = MEDIASUBTYPE_PCM;
    //            qDebug() << "1111";
    //    hr = sg->SetMediaType(&amt);
    //        qDebug() << "hr = " << hr;
    //    sg.Release();


    m_vu->setGrabber(m_sg);
    return hr;
}


void PlaybackGraph::updateVideoSize()
{
    if (m_vw)
         m_vw->updateSize();
}

void PlaybackGraph::onSize()
{
    if (m_vw)
         m_vw->updateSize();
}

void PlaybackGraph::onMove()
{
    if (m_vw)
         m_vw->updateMove();
}

void PlaybackGraph::play()
{
    if (m_mc)
    {
        m_mc->Run();
    //    qDebug() << "hr: " << hr;
       // setPosition(getPosition());
    }
}

void PlaybackGraph::pause()
{
    if (m_mc)
         m_mc->Pause();

   //  m_vu->onPause();
}

void PlaybackGraph::stop()
{
    if (m_mc)
         m_mc->Stop();

   // m_vu->onStop();
}

IGraph::Graph_State PlaybackGraph::getState()
{
    return BaseGraph::getState();
}

__int64 PlaybackGraph::getDuration()
{
    __int64 duration;

    m_ms->GetDuration(&duration);
    return duration;
}

__int64 PlaybackGraph::getPosition()
{
    if (!m_ms)
        return 0;
    __int64 position;

    HRESULT hr = m_ms->GetCurrentPosition(&position);

    if (FAILED(hr))
        return 0;
    return position;
}
int PlaybackGraph::getFrameNumber()
{
    if (!m_vw)
        return -1;

    REFTIME avg;
    m_vw->getAvgTimePerFrame(&avg);
    __int64 frameDuration = avg * 10000000;
    __int64 pos = getPosition();
    return pos / frameDuration;
}

// Setting position based on time
void PlaybackGraph::setPosition(__int64 position)
{
     if (!m_ms)
          return;

     __int64 start = position;
     __int64 stop = 0;
     m_ms->SetPositions(&start, AM_SEEKING_AbsolutePositioning ,&stop,0);
}


// Setting position based on video frame number
void PlaybackGraph::setPosition(int frame)
{
    if (m_vw)
    {
        REFTIME avgTimePerFrame;
        m_vw->getAvgTimePerFrame(&avgTimePerFrame);
        //qDebug() << "Average time per frame (in msec)" << avgTimePerFrame;

        __int64 position = frame * avgTimePerFrame * 10000000;
        //qDebug() << "Position" << position;

        setPosition(position);
    }
}

void PlaybackGraph::processEvents()
{
    if (m_mex)
    {
      long code;
      LONG_PTR prm1, prm2;
      while (SUCCEEDED(m_mex->GetEvent(&code, &prm1, &prm2, 0)))
      {
          m_mex->FreeEventParams(code, prm1, prm2);
          switch (code)
          {
          case EC_COMPLETE:
              onComplete();
              break;

          case EC_USERABORT: // Fall through.
          case EC_ERRORABORT:
              return;
          }
      }
   }
}

void PlaybackGraph::onComplete()
{
    stop();
}

void PlaybackGraph::setVolume(short level)
{
    if (m_ba)
    {
       // int val = (double)level  * (10000.0 / 65535.0) - 10000;
        int val = level;
        HRESULT hr = m_ba->put_Volume(val);
        //m_logger->AddLogMessage(INFO, "PlaybackGraph::setVolume(), level=" +  QString::number(level) + " val=" + QString::number(val) + " hr=" + QString::number(hr));
    }
}

void PlaybackGraph::getVUValues(int channel, int *avg, int *peak)
{
    m_vu->getValues(channel, avg, peak);
}


HBITMAP PlaybackGraph::getLastFrame()
{
    if (!m_vw)
        return 0;

    return m_vw->getLastFrame();
}

void PlaybackGraph::addOverlayText(int, int, QString)
{

}

HRESULT PlaybackGraph::applyOverlay()
{
    return S_OK;
}

void PlaybackGraph::clearOverlay()
{

}

HRESULT PlaybackGraph::getAudioRenderers(QStringList &)
{
    return E_NOTIMPL;
}

HRESULT PlaybackGraph::setAudioRenderer(QString)
{
    return E_NOTIMPL;
}

HRESULT PlaybackGraph::showVideoWindow(bool show)
{
    HRESULT hr = S_FALSE;
    if (m_vw)
        hr = m_vw->showWindow(show);
    return hr;
}
