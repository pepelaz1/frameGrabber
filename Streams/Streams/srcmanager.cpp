#include "srcmanager.h"
#include "informer_graph.h"

SrcManager::SrcManager()
{

}

SrcManager::~SrcManager()
{
}

HRESULT SrcManager::getVideoSources(QStringList &list)
{
    return getFiltersInCategory(CLSID_VideoInputDeviceCategory, list);
}

HRESULT SrcManager::getAudioSources(QStringList &list)
{
    return getFiltersInCategory(CLSID_AudioInputDeviceCategory, list);
}

HRESULT SrcManager::setVideoSource(QString src)
{
    m_vsrc = src;
    return S_OK;
}

HRESULT SrcManager::setAudioSource(QString src)
{
    m_asrc = src;
    return S_OK;
}

QString SrcManager::getVideoSource()
{
    return m_vsrc;
}

QString SrcManager::getAudioSource()
{
    return m_asrc;
}

HRESULT SrcManager::prepareSource(QString sourceName, CComPtr<IAMStreamConfig> &result)
{
    HRESULT hr = S_OK;
    m_informer->reset();

    hr = m_informer->build();
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "SrcManager::prepareSource(), failed to build informer graph hr=" + QString::number(hr));
        return hr;
    }

    CComPtr<IBaseFilter> flt;
    hr = m_informer->addFilter(sourceName, L"Video source", flt, CLSID_VideoInputDeviceCategory);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "SrcManager::prepareSource(), failed to add video source filter to informer graph hr=" + QString::number(hr));
        return hr;
    }


    ICaptureGraphBuilder2 *cb = m_informer->getCaptureBuilder();
    CComPtr<IAMStreamConfig> cfg;
    hr = cb->FindInterface(
        &PIN_CATEGORY_CAPTURE,
        &MEDIATYPE_Video,
        flt,
        IID_IAMStreamConfig,
        (void **)&cfg);
    if (FAILED(hr))
    {
        //m_logger->AddLogMessage(CRITICAL, "SrcManager::prepareSource(), unable to find IAMStreamConfig interface for capture branch in informer graph hr=" + QString::number(hr));
        return hr;
    }

    result = cfg;
    return hr;
}

HRESULT SrcManager::getVideoSubtypes(QString sourceName, QStringList &lst)
{
    HRESULT hr = S_OK;
    CComPtr<IAMStreamConfig> cfg;
    hr = prepareSource(sourceName, cfg);
    if (FAILED(hr))
    {
       //m_logger->AddLogMessage(CRITICAL, "SrcManager::getVideoSubtypes(), failed to prepare source device hr=" + QString::number(hr));
       return hr;
    }

    int cnt = 0;
    int size = 0;
    hr = cfg->GetNumberOfCapabilities(&cnt, &size);
    if (SUCCEEDED(hr))
    {
        unsigned char *buff = new unsigned char[size];
        for (int i = 0; i < cnt; i++)
        {
            AM_MEDIA_TYPE *amt;
            hr = cfg->GetStreamCaps(i, &amt, buff);
            if (SUCCEEDED(hr))
            {
                QString subtype = GuidNames[amt->subtype];
                subtype = subtype.replace("MEDIASUBTYPE_", "");
                if (lst.contains(subtype) == false)
                    lst.push_back(subtype);

                DeleteMediaType(amt);
            }
        }
        delete buff;
    }
    return hr;
}


HRESULT SrcManager::getVideoResolutions(QString sourceName, QString subtype, QStringList &lst)
{
    HRESULT hr = S_OK;
    CComPtr<IAMStreamConfig> cfg;
    hr = prepareSource(sourceName, cfg);
    if (FAILED(hr))
    {
       //m_logger->AddLogMessage(CRITICAL, "SrcManager::getVideoResolutions(), failed to prepare source device hr=" + QString::number(hr));
       return hr;
    }

    int cnt = 0;
    int size = 0;
    hr = cfg->GetNumberOfCapabilities(&cnt, &size);
    if (SUCCEEDED(hr))
    {
        unsigned char *buff = new unsigned char[size];
        for (int i = 0; i < cnt; i++)
        {
            AM_MEDIA_TYPE *amt;
            hr = cfg->GetStreamCaps(i, &amt, buff);
            if (SUCCEEDED(hr))
            {
                QString st = GuidNames[amt->subtype];
                st = st.replace("MEDIASUBTYPE_", "");
                if ( st == subtype)
                {
                    if (amt->formattype == FORMAT_VideoInfo)
                   {
                       VIDEOINFO *vi = (VIDEOINFO *)amt->pbFormat;
                       QString s = QString::number(vi->bmiHeader.biWidth) + "x" +  QString::number(vi->bmiHeader.biHeight);
                       lst.push_back(s);
                   }
                   else if (amt->formattype == FORMAT_VideoInfo2)
                   {
                       VIDEOINFOHEADER2 *vi = (VIDEOINFOHEADER2 *)amt->pbFormat;
                       QString s = QString::number(vi->bmiHeader.biWidth) + "x" +  QString::number(vi->bmiHeader.biHeight);
                       lst.push_back(s);
                   }
                }
                DeleteMediaType(amt);
            }
        }
        delete buff;
    }
    return hr;
}
